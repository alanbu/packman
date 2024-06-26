/*********************************************************************
* This file is part of PkgTest (PackMan unit testing)
*
* Copyright 2014-2020 AlanBuckley
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*****************************************************************************/
/* File to provide the stubs and registering of the
 * test functions provided to the LUA scripts for
 * the pkg functions
 */

#include "TestRunner.h"
#include "Packages.h"

#include "luacc/lua.h"
#include "luacc/lualib.h"
#include "luacc/lauxlib.h"

#include "libpkg/pkgbase.h"
#include "libpkg/zipfile.h"
#include "libpkg/filesystem.h"
#include "libpkg/env_checker.h"
#include "libpkg/component_update.h"

#include "tbx/hourglass.h"
#include "tbx/path.h"

#include <sstream>
#include <cstring>
#include <algorithm>

// Helper functions
pkg::binary_control extract_control(const std::string& pathname);
const pkg::binary_control &available_control(const std::string &pkgname);


/**
 * Get actual path for a logical path
 *
 * path = pkg.path(<logical_path>)
 *
 * Fails if <logical_path> is invalid
 */
static int l_pkg_path(lua_State* L)
{
  const char *logical_path = lua_tostring(L, 1);
  pkg::pkgbase *pkg_base = Packages::instance()->package_base();
  std::string actual_path = pkg_base->paths()(logical_path, "");

  test_log() << "lookup path '" << logical_path << "' result '" << actual_path << "'" << std::endl;
  lua_pushstring(L, actual_path.c_str());

  return 1; // Count of returned values
}

/**
 * Add a package from the local disc from the <PkgTest$Dir>.Packages directories
 *
 * pkg.add(<package file name>);
 *
 * Fails if package file can not be found or is invalid
 */
static int l_pkg_add(lua_State *L)
{
	const char *pkg_path = lua_tostring(L,1);

	test_log() << "Adding local package " << pkg_path << std::endl;

    pkg::pkgbase *package_base = Packages::instance()->package_base();
	tbx::Hourglass hg;
	std::string pathname("<PkgTest$Dir>.Packages.");
	pathname += pkg_path;

  std::string errmsg;

	try
	{
		if (pkg::object_type(pathname) == 0)
			throw std::runtime_error("file missing from Packages directory");

		pkg::binary_control ctrl = extract_control(pathname);
		std::string pkgname(ctrl.pkgname());
		std::string pkgvrsn(ctrl.version());
		if (pkgname.length() && pkgvrsn.length())
		{
			std::ostringstream out;
			out << pkg::object_length(pathname);
			ctrl["Size"] = out.str();

			std::string cache_pathname = package_base->cache_pathname(pkgname,
					pkgvrsn, ctrl.environment_id());
			tbx::Path(pathname).copy(cache_pathname,tbx::Path::COPY_FORCE);
			package_base->control().insert(ctrl);
			package_base->control().commit();
		} else
		{
			errmsg = "Not a package (Package name or version not found)";
		}
	} catch (pkg::zipfile::not_found& ex)
	{
		errmsg = "Not a package (control file not found)";
	} catch (std::exception& ex)
	{
		errmsg = "Failed to load package: ";
		errmsg += ex.what();
	}

	if (errmsg.empty())
	{
		return 0;
	} else
	{
		lua_pushstring(L, errmsg.c_str());
		return lua_error(L);
	}
}

/**
 * Mark package to be installed
 *
 * pkg.install(<package name>)
 *
 * Fails if package doesn't exist or is already installed
 */
static int l_pkg_install(lua_State *L)
{
	const char *pkgname = lua_tostring(L,1);

	test_log() << "Install " << pkgname << std::endl;

	try
	{
		const pkg::binary_control &bctrl = available_control(pkgname);
		pkg::pkgbase *pkg_base = Packages::instance()->package_base();

		if (pkg_base->curstat()[pkgname].state() == pkg::status::state_installed)
    		{
			lua_pushstring(L, "Package is already installed");
			return lua_error(L);
		} else
		{
		    	Packages::instance()->select_install(&bctrl);
    		}
	} catch(std::exception &ex)
	{
		std::string msg("Failed to find package to install '");
		msg += pkgname;
		msg += "': ";
		msg += ex.what();

		test_log() << msg << std::endl;

		lua_pushstring(L, msg.c_str());
                return lua_error(L);
	}
    

   	return 0;
}

/**
 * Mark package to be upgraded
 *
 * pkg.upgrade(<package name>)
 *
 * Fails if package doesn't exist or is not already installed
 * or the latest version is already installed.
 */
static int l_pkg_upgrade(lua_State *L)
{
	const char *pkgname = lua_tostring(L,1);

	test_log() << "Upgrade " << pkgname << std::endl;

	const pkg::binary_control &bctrl = available_control(pkgname);
	pkg::pkgbase *pkg_base = Packages::instance()->package_base();

    std::string errmsg;
    if (pkg_base->curstat()[pkgname].state() != pkg::status::state_installed)
    {
       errmsg = "Package is not installed";
    } else if (pkg::version(bctrl.version()) <= pkg::version(pkg_base->curstat()[pkgname].version()))
    {
    	errmsg = "Latest version of package is already installed";
    	errmsg += " (installed " + pkg_base->curstat()[pkgname].version();
    	errmsg += ", upgrade version " + bctrl.version() + ")";
    } else
    {
    	Packages::instance()->select_install(&bctrl);
    }

    if (errmsg.empty())
    {
    	return 0;
    } else
    {
    	lua_pushstring(L, errmsg.c_str());
    	return lua_error(L);
    }
}

/**
 * Mark package to be removed
 *
 * pkg.remove(<package name>)
 *
 * Fails if package doesn't exist or is not installed
 */
static int l_pkg_remove(lua_State *L)
{
	const char *pkgname = lua_tostring(L,1);

	test_log() << "Remove " << pkgname << std::endl;

	const pkg::binary_control &bctrl = available_control(pkgname);
	pkg::pkgbase *pkg_base = Packages::instance()->package_base();

    if (pkg_base->curstat()[pkgname].state() != pkg::status::state_installed)
    {
		lua_pushstring(L, "Package is not installed");
		return lua_error(L);
    } else
    {
    	Packages::instance()->select_remove(&bctrl);
    }

   	return 0;
}

/**
 * Withdraw package from list of packages to be commited
 *
 * pkg.withdraw(<package name>)
 *
 * Fails if package doesn't exist
 * No error if package isn't selected for install/remove
 */
static int l_pkg_withdraw(lua_State *L)
{
	const char *pkgname = lua_tostring(L,1);

	test_log() << "Withdraw " << pkgname << std::endl;
    Packages::instance()->deselect(pkgname);

   	return 0;
}

/**
 * Commit new packages status
 *
 * pkg.commit()
 *
 * Fails if the commit fails
 */
static int l_pkg_commit(lua_State *L)
{
	test_log() << "Commit" << std::endl;

	try
	{
		tr()->commit();
	} catch(std::exception &e)
	{
		lua_pushstring(L, e.what());
		return lua_error(L);
	}

	return 0;
}

/**
 * Commit new packages status expecting failure
 *
 * pkg.commit_fail("reason")
 *
 * reason is:
 *  "conflicts" for file conflicts
 *  full message of failure error
 *  text with *, where * is used as a simple wildcard for 0 or more characters.
 *
 * Fails if the commit fails
 */
static int l_pkg_commit_fail(lua_State *L)
{
	const char *fail = lua_tostring(L,1);

	test_log() << "Commit expecting failure: " << fail << std::endl;

	try
	{
		tr()->commit_fail(fail);
	} catch(std::exception &e)
	{
		lua_pushstring(L, e.what());
		return lua_error(L);
	}

	return 0;
}

/**
 * Return the control record for a given package
 *
 * ctrl = pkg.control(<package name>)
 *
 * Fails if <package name> isn't a control record
 */
static int l_pkg_control(lua_State *L)
{
	const char *pkgname = lua_tostring(L,1);
	const pkg::binary_control &bctrl = available_control(pkgname);

	lua_newtable(L);
	int top = lua_gettop(L);

	for (pkg::binary_control::const_iterator i = bctrl.begin(); i != bctrl.end(); ++i)
	{
	    const char* key = i->first.c_str();
	    const char* value = i->second.c_str();
	    lua_pushlstring(L, key, i->first.size());
	    lua_pushlstring(L, value, i->second.size());
	    lua_settable(L, top);
	}

	return 1;
}

/**
 * Dump information to the log to help debugging
 *
 * pkg.dump(<what>)
 * <what> is currently just "available" for the available packages list
 *
 * Fails if <what> is invalid
 */
static int l_pkg_dump(lua_State *L)
{
	const char *what = lua_tostring(L,1);
	if (std::strcmp(what, "available") == 0)
	{
		pkg::pkgbase *pkg_base = Packages::instance()->package_base();
		pkg::binary_control_table &btable = pkg_base->control();
		pkg::binary_control_table::const_iterator c;

		// First check package exists
		for (c = btable.begin(); c != btable.end(); ++c)
		{
			test_log() << "Package: " << c->first.pkgname << ", Version: " 
				<< (std::string)c->first.pkgvrsn << ", Environment: " << c->first.pkgenv << std::endl;
		}
	} else
	{
		lua_pushstring(L, "Invalid argument to dump command");
		return lua_error(L);
	}
	return 1;
}

/**
 * Helper to parse a csv list to a set
 * @param csv_list comma seperated list to convert
 * @param out_set set updated with list of values
 */
static void csv_to_set(const char *csv_list, std::set<std::string> &out_set)
{
	const char *s = csv_list;
	const char *e;
	std::string item;
	do
	{
		while (*s == ' ') s++;
	  e = strchr(s, ',');
		if (e == nullptr)
		{
			item.assign(s);
		} else
		{
			item.assign(s, e - s);
			s = e + 1;
		}
		while (!item.empty() && item.back() == ' ')	item.erase(item.size()-1);
		if (!item.empty()) out_set.insert(item);
	} while (e);
}

/**
 * Set environment for packages
 *
 * pkg.environment(<env>, <mod>)
 * <env> is the package environment to use (comma separated list) or "" for the default.
 * <mod> is an optional list of modules
 */
static int l_pkg_environment(lua_State *L)
{
	const char *env_list = lua_tostring(L,1);
	const char *mod_list = luaL_optstring(L,2,NULL);
	test_log() << "Changing environment to '" << env_list << "'" << std::endl;
	if (mod_list && *mod_list) test_log() << "Changed environment modules to '" << mod_list << "'";
	std::set<std::string> envs, mods;
	csv_to_set(env_list, envs);
	if (mod_list && *mod_list)
	{
		std::string lower_mod_list(mod_list);
		std::transform(lower_mod_list.begin(), lower_mod_list.end(), lower_mod_list.begin(),
				                   [](unsigned char c){ return std::tolower(c); }
		);
		csv_to_set(lower_mod_list.c_str(), mods);
	} 

	pkg::env_checker::instance()->override_environment(envs, mods);

	return 1;
}

/**
 * Reset environment for packages to default
 *
 * pkg.reset_environment()
 */
static int l_pkg_reset_environment(lua_State *L)
{
	pkg::env_checker::instance()->clear_environment_overrides();
	test_log() << "Environment reset" << std::endl;
	return 1;
}

/**
 * Set install path for a package
 * 
 * Will select it for installation if not already selected
 * 
 * pkg.set_install_path(<pkgname>, <install_path>)
*/
static int l_pkg_set_install_path(lua_State *L)
{
	const char *pkgname = lua_tostring(L,1);
	const char *path = lua_tostring(L,2);

	test_log() << "Set install path for " << pkgname << " to " << path << std::endl;

	try
	{
		const pkg::binary_control &bctrl = available_control(pkgname);
		pkg::pkgbase *pkg_base = Packages::instance()->package_base();

		if (pkg_base->curstat()[pkgname].state() == pkg::status::state_installed)
		{
			lua_pushstring(L, "Package is already installed");
			return lua_error(L);
		} else
		{
	    	Packages::instance()->select_install(&bctrl);
    	}

		auto foundComp = bctrl.find("Components");
		if (foundComp == bctrl.end())
		{
			lua_pushstring(L, "Package does not have a components field");
			return lua_error(L);
		}
		pkg::component_update comp_update(Packages::instance()->package_base()->component_update_pathname());
		pkg::component comp(foundComp->second);
		comp.path(path);
		comp_update.insert(comp);
		comp_update.commit();
	} catch(std::exception &ex)
	{
		std::string msg("Failed to find/update package component path for '");
		msg += pkgname;
		msg += "': ";
		msg += ex.what();

		test_log() << msg << std::endl;

		lua_pushstring(L, msg.c_str());
        return lua_error(L);
	}

	return 0;
}

static const struct luaL_reg pkglib [] = {
   {"path", l_pkg_path},
   {"add", l_pkg_add},
   {"install", l_pkg_install},
   {"upgrade", l_pkg_upgrade},
   {"remove", l_pkg_remove},
   {"withdraw", l_pkg_withdraw},
   {"commit", l_pkg_commit},
   {"commit_fail", l_pkg_commit_fail},
   {"control", l_pkg_control},
   {"dump", l_pkg_dump},
   {"environment", l_pkg_environment},
   {"reset_environment", l_pkg_reset_environment},
   {"set_install_path", l_pkg_set_install_path},
   {NULL, NULL}  /* sentinel */
 };

/**
 * Load the test library into the lua state
 */
int loadlib_pkg(lua_State *L)
{
	luaL_openlib(L, "pkg", pkglib, 0);
	return 1;
}

/**
 * Extract the control record from the given file
 */
pkg::binary_control extract_control(const std::string& pathname)
{
	pkg::create_directory("PkgTestDisc:Temp");
	pkg::zipfile zf(pathname);
	zf.extract("RiscPkg/Control","PkgTestDisc:Temp.Control");
	std::ifstream ctrlfile("PkgTestDisc:Temp.Control");
	pkg::binary_control ctrl;
	ctrlfile >> ctrl;
	return ctrl;
}

/**
 * Get the binary control record for a package
 *
 * @param pkgname name of package to check
 * @returns binary control pointer
 * @throws std::runtime_error if package isn't in the available list
 */
const pkg::binary_control &available_control(const std::string &pkgname)
{
	pkg::pkgbase *pkg_base = Packages::instance()->package_base();
	pkg::binary_control_table &btable = pkg_base->control();
	pkg::binary_control_table::const_iterator c;

	// First check package exists
	for (c = btable.begin(); c != btable.end() && c->first.pkgname != pkgname; ++c);
	if (c == btable.end()) throw std::runtime_error("Package not in available list");

	// Return latest version (not necessarily what was found above)
	return btable[pkgname];
}
