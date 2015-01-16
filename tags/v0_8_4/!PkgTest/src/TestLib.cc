/*********************************************************************
* Copyright 2014 Alan Buckley
*
* This file is part of PkgTest (PackMan unit testing)
*
* PackMan is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* PkgTest is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with PackMan. If not, see <http://www.gnu.org/licenses/>.
*
*****************************************************************************/
/**
 * File to provide the stubs and registering of the
 * test functions provided to the LUA scripts
 */

#include "TestRunner.h"
#include "Packages.h"

#include "luacc/lua.h"
#include "luacc/lualib.h"
#include "luacc/lauxlib.h"

#include "libpkg/pkgbase.h"
#include "libpkg/auto_dir.h"
#include "libpkg/filesystem.h"
#include "libpkg/module_info.h"
#include "libpkg/zipfile.h"

#include "tbx/hourglass.h"
#include "tbx/path.h"

// Helper functions
void erase_status(pkg::status_table &table, const std::string &pkgname);
std::string src_to_zip(const std::string& src_pathname);
void recursive_delete(const std::string &dirname);

// Helpers from PkgLib.cc
extern const pkg::binary_control &available_control(const std::string &pkgname);

/**
 * Send message to log and window showing tests
 *
 * test.message(<text>)
 */
static int l_message(lua_State* L)
{
  const char *msg = lua_tostring(L, 1);
  TestRunner::instance()->message(msg);

  return 0; // Count of returned values
}

/**
 * Test has failed so stop the script
 *
 * test.fail(<message>)
 */
static int l_fail(lua_State *L)
{
  return lua_error(L);
}

/**
 * Fast erase of package from disc.
 *
 * This uses the installed Files list to delete all the
 * files installed by a package and then deletes it
 * from the package list and package tables.
 *
 * It is not an error if the package is not installed.
 *
 * test.erase(<package name>)
 */
static int l_erase(lua_State *L)
{
	const char *pkgname = lua_tostring(L,1);
	test_log() << "Erase " << pkgname << std::endl;

    pkg::pkgbase *pkg_base = Packages::instance()->package_base();
    pkg::path_table &paths = pkg_base->paths();
    pkg::status_table &curstat = pkg_base->curstat();
    pkg::status_table &selstat = pkg_base->selstat();
    pkg::binary_control_table &bctrl = pkg_base->control();

    // First try to remove any installed files
    std::set<std::string> files;
    std::string info_pathname = pkg_base->info_pathname(pkgname);
    std::string mf_filename = info_pathname + ".Files";
    std::ifstream dst_in(mf_filename.c_str());

    if (dst_in)
    {
		dst_in.peek();
		while (dst_in&&!dst_in.eof())
		{
			std::string line;
			std::getline(dst_in,line);
			if (line.length()) files.insert(line);
			dst_in.peek();
		}
		dst_in.close();

		// Use auto dir to automatically delete empty directories
		pkg::auto_dir ad;
		for (std::set<std::string>::iterator f = files.begin(); f != files.end(); ++f)
		{
			std::string file_name = paths(*f, pkgname);
			pkg::force_delete(file_name);
			ad(file_name);
		}
    }

    try
    {
    	if (tbx::Path(info_pathname).directory()) recursive_delete(pkg_base->info_pathname(pkgname));
    } catch(std::exception &e)
    {
    	lua_pushstring(L, ("Failed to delete '" + info_pathname + "'").c_str());
    	return lua_error(L);
    }

    // Remove from status tables
    erase_status(curstat, pkgname);
    erase_status(selstat, pkgname);

    // Remove from available list

    // Check it's there first
    pkg::binary_control_table::const_iterator c;
    for (c = bctrl.begin(); c != bctrl.end() && c->first.pkgname != pkgname; ++c);
    if (c != bctrl.end())
    {
    	// Found package so overwrite available file and then reload it
    	std::ofstream bf(pkg_base->available_pathname().c_str());
    	if (bf)
    	{
    	    for (c = bctrl.begin(); c != bctrl.end(); ++c)
    	    {
    	    	if (c->first.pkgname != pkgname)
    	    	{
    	    		bf << c->second << std::endl;
    	    	}
    	    }
    	    bf.close();

    	    // Now reload cleaned package
    	    bctrl.update();
    	}
    }

	return 0;
}

/**
 * Check the version of a module file
 *
 * test.check_module(<path to module>, <version>)
 *
 * Fails if module missing, invalid or version doesn't match
 */
static int l_check_module(lua_State *L)
{
	const char *mod_path = lua_tostring(L,1);
	const char *version = lua_tostring(L,2);
	test_log() << "Check module " << mod_path << " version " << version << std::endl;

	pkg::module_info mod_info(mod_path);
	std::string errmsg;
	if (!mod_info.read_ok()) errmsg = "Missing or invalid module";
	else if (mod_info.version() != version) errmsg = "Module versions don't match, found " + mod_info.version();

	if (!errmsg.empty())
	{
		lua_pushstring(L, errmsg.c_str());
		return lua_error(L);
	}

	return 0;
}

/**
 * Check if a file has been deleted
 *
 * test.check_file_deleted(<path to file>)
 *
 * Fails if file exists on the file system
 */
static int l_check_file_deleted(lua_State *L)
{
	const char *file_path = lua_tostring(L,1);

	test_log() << "Checking if file '" << file_path << "' has been deleted" << std::endl;
	if (pkg::object_type(file_path) == 0)
	{
		return 0;
	} else
	{
		lua_pushstring(L, "file still exists");
		return lua_error(L);
	}
}

/**
 * Extract a file from a package zip file.
 *
 * extract.file(<package file>, <logical path of file to extract>)
 */
static int l_extract_file(lua_State *L)
{
	const char *pkg_path = lua_tostring(L,1);
	const char *logical_name = lua_tostring(L,2);
	pkg::pkgbase *pkg_base = Packages::instance()->package_base();
	std::string destination = pkg_base->paths()(logical_name, "");

	test_log() << "Extracting file '" << logical_name << "' from package '" << pkg_path << "' to '" << destination << "'" << std::endl;

	tbx::Hourglass hg;
	std::string pathname("<PkgTest$Dir>.Packages.");
	pathname += pkg_path;

	try
	{
		pkg::auto_dir ad;
		ad(destination);

		pkg::zipfile zf(pathname);
		zf.extract(src_to_zip(logical_name),destination);
	} catch (std::exception &e)
	{
		std::string msg("Failed to extract '");
		msg += logical_name;
		msg += "', error: ";
		msg += e.what();
    	lua_pushstring(L, msg.c_str());
    	return lua_error(L);
	}

	return 0;
}

/**
 * Check installed package version
 *
 * test.check_version(<package name>,<version>)
 */
static int l_check_version(lua_State *L)
{
	const char *pkgname = lua_tostring(L,1);
	const char *version = lua_tostring(L,2);

	test_log() << "Checking package '" << pkgname << "' is version " << version << "'" << std::endl;

    pkg::pkgbase *pkg_base = Packages::instance()->package_base();
    const pkg::status &stat = pkg_base->curstat()[pkgname];
    std::string errmsg;
    if (stat.state() != pkg::status::state_installed)
    {
        errmsg = "Package is not installed";
    } else if (stat.version() != version)
    {
    	errmsg = "Package is incorrect version " + stat.version();
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
 * Check package state
 *
 * test.check_stat(<package name>,<status>)
 */
static int l_check_state(lua_State *L)
{
	const char *pkgname = lua_tostring(L,1);
	const char *state = lua_tostring(L,2);

	test_log() << "Checking package '" << pkgname << "' state is '" << state << "'" << std::endl;

    pkg::pkgbase *pkg_base = Packages::instance()->package_base();
    const pkg::status &stat = pkg_base->curstat()[pkgname];
    std::string cstate;

    switch(stat.state())
    {
    case pkg::status::state_not_present: cstate = "not present"; break;
    case pkg::status::state_removed: cstate = "removed"; break;
    case pkg::status::state_half_unpacked: cstate = "half unpacked"; break;
    case pkg::status::state_unpacked: cstate = "unpacked"; break;
    case pkg::status::state_half_configured: cstate = "half configured"; break;
    case pkg::status::state_installed: cstate = "installed"; break;
    }

    if (cstate != state)
    {
    	std::string msg("Package is incorrect state '");
    	msg += cstate + "'";
    	lua_pushstring(L, msg.c_str());
    	return lua_error(L);
    }

    return 0;
}


/**
 * Check if a condition is true
 *
 * This function allows the use of any lua code to perform a check
 * which fails it the code results to false
 *
 * test.check(<description>, <lua check>, <error message if fails>)
 */
static int l_check(lua_State *L)
{
	const char *desc = lua_tostring(L,1);
	bool result = lua_toboolean(L,2);
	const char *errmsg = lua_tostring(L,3);

	test_log() << desc << std::endl;
	if (!result)
	{
		lua_pushstring(L, errmsg);
		return lua_error(L);
	}

	return 0;
}

/**
 * Checks that the control information for a package
 * matches the available list
 *
 * test.check_control_info(<package name>)
 */
static int l_check_control_info(lua_State *L)
{
	const char *pkgname = lua_tostring(L,1);
	const pkg::binary_control &bctrl = available_control(pkgname);
    pkg::pkgbase *pkg_base = Packages::instance()->package_base();

	std::string filename = pkg_base->info_pathname(pkgname) + ".Control";
	std::string errmsg;

	std::ifstream cf(filename.c_str());
	if (cf)
	{
		pkg::binary_control info_control;
		cf >> info_control;
		cf.close();
		bool matches = true;
		for (pkg::binary_control::const_iterator f = bctrl.begin(); f != bctrl.end() && matches; ++f)
		{
			std::string key = f->first;
			std::string value = f->second;
			if (info_control[key] != value)
			{
				errmsg = "Values don't match for " + key;
				errmsg+= " control '" + value;
				errmsg+= "' info '" + value + "'";
				matches = false;
			}
		}
		if (matches)
		{
			for (pkg::binary_control::const_iterator f = info_control.begin(); f != info_control.end() && matches; ++f)
			{
				std::string key = f->first;
				if (bctrl.find(key) == bctrl.end())
				{
					errmsg = "Extra field in info " + key;
					matches = false;
				}
			}
		}

	} else
	{
		errmsg = "unable to open control record at " + filename;
	}

	if (!errmsg.empty())
	{
		lua_pushstring(L, errmsg.c_str());
		return lua_error(L);
	}

	return 0;
}

/**
 * Pause the current test. This leaves it running in an
 * idle loop so that files/etc can be checked as an aid
 * to debugging
 */
static int l_pause(lua_State *L)
{
	const char *msg = lua_tostring(L,1);
	tr()->pause(msg);

	return 0;
}

static const struct luaL_reg testlib [] = {
   {"message", l_message},
   {"fail", l_fail},
   {"erase", l_erase},
   {"check_module", l_check_module},
   {"check_file_deleted", l_check_file_deleted},
   {"extract_file", l_extract_file},
   {"check_version", l_check_version},
   {"check", l_check},
   {"check_control_info", l_check_control_info},
   {"check_state", l_check_state},
   {"pause", l_pause},
   {NULL, NULL}  /* sentinel */
 };


/**
 * Load the test library into the lua state
 */
int loadlib_test(lua_State *L)
{
	luaL_openlib(L, "test", testlib, 0);
	return 1;
}

/**
 * Erase a package from a status table
 */
void erase_status(pkg::status_table &table, const std::string &pkgname)
{
   pkg::status_table::const_iterator found = table.find(pkgname);
   if (found == table.end()) return; // Nothing to do

   // Reset status to null will remove it from the table
   table.insert(pkgname, pkg::status());
   table.commit();   // Update table to disc
   table.rollback(); // Make sure it's removed from the memory copy as well
}


/** Convert source pathname to zip file pathname.
 *
 * This routine taken from libpkg unpack.cc
 *
 * @param src_pathname the source pathname
 * @return the zip file pathname
 */
std::string src_to_zip(const std::string& src_pathname)
{
	std::string zip_pathname(src_pathname);
	for (unsigned int i=0;i!=zip_pathname.length();++i)
	{
		switch (zip_pathname[i])
		{
		case '/':
			zip_pathname[i]='.';
			break;
		case '.':
			zip_pathname[i]='/';
			break;
		}
	}
	return zip_pathname;
}


/**
 * Recursively delete directories
 */
void recursive_delete(const std::string &dirname)
{
	tbx::Path dirpath(dirname);
	std::vector<std::string> sub_dirs;
	for (tbx::PathInfo::Iterator f = tbx::PathInfo::begin(dirname); f != tbx::PathInfo::end(); ++f)
	{
		if (f->directory()) sub_dirs.push_back(f->name());
		else tbx::Path(dirpath, f->name()).remove();
	}

	for(std::vector<std::string>::iterator sd = sub_dirs.begin(); sd != sub_dirs.end(); ++sd)
	{
		recursive_delete(dirname + "." + *(sd));
	}
	dirpath.remove();
}
