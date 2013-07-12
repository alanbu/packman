/*********************************************************************
* Copyright 2009-2013 Alan Buckley
*
* This file is part of PackMan.
*
* PackMan is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* PackMan is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with PackMan. If not, see <http://www.gnu.org/licenses/>.
*
*****************************************************************************/
/*
 * Packages.cc
 *
 *  Created on: 23-Mar-2009
 *      Author: alanb
 */

#include "Packages.h"
#include "ErrorWindow.h"
#include "libpkg/pkgbase.h"
#include "libpkg/filesystem.h"
#include "libpkg/log.h"
#include <string>
#include <set>
#include <cstdlib>

#include "swis.h"
#include "tbx/swixcheck.h"
#include "tbx/stringutils.h"

using namespace std;

Packages *Packages::_instance = 0;

Packages g_packages; // Single instance of the packages

Packages::Packages() :
     _package_base(0),
     _upgrades_available(DONT_KNOW),
     _logging(false)
{
	_instance = this;
}

Packages::~Packages()
{
	_instance = 0;
}

/**
 * Ensure packages have been loaded
 */
bool Packages::ensure_package_base()
{
   	if (!_package_base)
	{
		try
		{
			// Do not use distribution master of package database.
			std::string apath=pkg::canonicalise("<Packages$Dir>");
			std::string dpath=
				pkg::canonicalise("<PackMan$Dir>.Resources.!Packages");
			if ((apath!=dpath)&&(pkg::object_type(apath)!=0))
			{
				// If the package database exists, but does not contain a
				// Paths file, and a Paths file does exist in the choices
				// directory (as used by previous versions of RiscPkg)
				// then copy it across.
				string pb_ppath=apath+string(".Paths");
				string ch_ppath("Choices:RiscPkg.Paths");
				if ((pkg::object_type(pb_ppath)==0)&&
					(pkg::object_type(ch_ppath)!=0))
				{
					pkg::copy_object(ch_ppath,pb_ppath);
				}

				// Attempt to access package database.
				_package_base=new pkg::pkgbase("<Packages$Dir>","<PackMan$Dir>.Resources",
					"Choices:PackMan");

				// Ensure that default paths are present, unless they
				// have been explicitly disabled by the user.
				bool paths_changed=_package_base->paths().ensure_defaults();
				if (paths_changed) _package_base->paths().commit();

			}
		} catch(...)
		{
			// Just delete as we will be shown the create package dialog
			delete _package_base;
			_package_base=0;
		}
	}
	return (_package_base != 0);
}

/**
 * Get cached list of unique package names.
 *
 * The list is different from the order in the binary control
 * table as the names are put in case insensitive sort order
 *
 * This is calculate the first time it is requested and
 * then again after reset_package_list is called
 */
const std::vector<std::string> &Packages::package_list()
{
	if (_package_list.empty())
	{
		const pkg::binary_control_table& ctrltab = _package_base->control();
		std::string prev_pkgname = "";

		for (pkg::binary_control_table::const_iterator i=ctrltab.begin();
			 i !=ctrltab.end(); ++i)
		{
			  std::string pkgname=i->first.pkgname;
			  if (pkgname!=prev_pkgname)
			  {
				  prev_pkgname=pkgname;
				  int i = (int)_package_list.size() - 1;
				  while (i >= 0 && tbx::compare_ignore_case(pkgname, _package_list[i]) < 0) i--;
				  if (i < (int)_package_list.size() - 1)
				  {
					  _package_list.insert(_package_list.begin() + i + 1, pkgname);
				  } else
				  {
					  _package_list.push_back(pkgname);
				  }
			  }
		}
	}

	return _package_list;
}

/**
 * Reset cached list of package names
 */
void Packages::reset_package_list()
{
	_package_list.clear();
}


/**
 * Return a sorted, comma separated list of all the sections
 * in the given packages
 */
std::string Packages::sections()
{
	if (_sections.empty())
	{
		const pkg::binary_control_table& ctrltab = _package_base->control();
		std::set<std::string> section_set;

		pkg::control::key_type section_key("Section");

		for (pkg::binary_control_table::const_iterator i=ctrltab.begin();
			 i !=ctrltab.end(); ++i)
		{
		    const pkg::binary_control &ctrl = i->second;

			pkg::control::const_iterator s = ctrl.find(section_key);
			if (s != ctrl.end()) section_set.insert((*s).second);
		}

		for (std::set<std::string>::iterator entry = section_set.begin();
		     entry != section_set.end(); ++entry)
		{
			_sections += (*entry);
			_sections += ",";
		}
		// Remove extra comma
		if (!_sections.empty()) _sections.erase(_sections.length()-1);
	}

	return _sections;
}

/**
 * Clear selection state.
 *
 * This resets the selection state to match the base state so nothing
 * is waiting for a commit.
 */
void Packages::clear_selection()
{
	pkg::status_table & seltable = _package_base->selstat ();

	pkg::status_table::const_iterator i;
	std::set < std::string > selected;

	// Get list of packages with there state and clear any old selection
	for (i = seltable.begin(); i != seltable.end(); ++i)
	{
		pkg::status curstat = _package_base->curstat ()[i->first];
		if (curstat != i->second)
		{
			selected.insert(i->first);
		}
	}

	for (std::set<std::string>::const_iterator reseti = selected.begin();
			reseti != selected.end(); ++reseti)
	{
		 pkg::status curstat = _package_base->curstat ()[*reseti];
		 seltable.insert(*reseti, curstat);
	}

	_package_base->fix_dependencies(selected);
	_package_base->remove_auto();
}

/**
 * Select a package for install or upgrade
 *
 * @param bctrl binary control record of package to install/upgrade
 */
void Packages::select_install(const pkg::binary_control *bctrl)
{
	const std::string &pkgname = bctrl->pkgname();
	std::set<std::string> seed;
	pkg::status curstat = _package_base->curstat()[pkgname];
	pkg::status selstat = _package_base->selstat()[pkgname];
	selstat.state(pkg::status::state_installed);
	if (curstat.state() == pkg::status::state_installed)
	{
		// Upgrade so need version as well
		const pkg::control& ctrl=_package_base->control()[pkgname];
		selstat.version(ctrl.version());
	}
	_package_base->selstat().insert(pkgname,selstat);
	seed.insert(pkgname);
	_package_base->fix_dependencies(seed);
	_package_base->remove_auto();
}

/**
 * Select a package to remove
 *
 * @param bctrl binary control record of package to removee
 */
void Packages::select_remove(const pkg::binary_control *bctrl)
{
	const std::string &pkgname = bctrl->pkgname();
	std::set<std::string> seed;
	pkg::status curstat = _package_base->curstat()[pkgname];
	pkg::status selstat = _package_base->selstat()[pkgname];
	selstat.state(pkg::status::state_removed);
	_package_base->selstat().insert(pkgname,selstat);
	seed.insert(pkgname);
	_package_base->fix_dependencies(seed);
	_package_base->remove_auto();
}

/**
 * Select all packages that can be upgraded.
 *
 * @returns true if upgrades are available
 */
bool Packages::select_upgrades()
{
	if (_upgrades_available == NO) return false;

	pkg::status_table &seltable = _package_base->selstat ();
	pkg::status_table &curtable = _package_base->curstat();
	std::set < std::string > seed;

    // Select all upgrades
    const pkg::binary_control_table& ctrltab = _package_base->control();
    std::string prev_pkgname;

    for (pkg::binary_control_table::const_iterator i=ctrltab.begin();
	 i !=ctrltab.end(); ++i)
    {
	   std::string pkgname=i->first.pkgname;
	   if (pkgname!=prev_pkgname)
	   {
		  // Don't use i->second for ctrl as it may not be the latest version
		  // instead look it up.
		  prev_pkgname=pkgname;

          pkg::status curstat=curtable[pkgname];
		  pkg::status selstat=seltable[pkgname];
		  if (selstat.state()>=pkg::status::state_installed)
		  {
		 	 const pkg::control& ctrl=ctrltab[pkgname];
			 selstat.version(ctrl.version());
			 if (_upgrades_available != YES)
			 {
				  pkg::version inst_version(curstat.version());
				  pkg::version cur_version(ctrl.version());

				  if (inst_version < cur_version)
				  {
					  _upgrades_available = YES;
				  }
			 }
			 if (_upgrades_available == YES)
			 {
				 _package_base->selstat().insert(pkgname,selstat);
				 seed.insert(pkgname);
			 }
		  }
	   }
    }

	_package_base->fix_dependencies (seed);
	_package_base->remove_auto ();

	return (_upgrades_available == YES);
}

/**
 * Remove package from the selection
 *
 * @param pkgname name of package to remove
 */
void Packages::deselect(const std::string &pkgname)
{
	std::set<std::string> seed;
	pkg::status curstat = _package_base->curstat()[pkgname];
	pkg::status selstat = _package_base->selstat()[pkgname];

	if (selstat != curstat)
	{
		selstat = curstat;
		_package_base->selstat().insert(pkgname,selstat);
		seed.insert(pkgname);
		_package_base->fix_dependencies(seed);
		_package_base->remove_auto();
	}
}

/**
 * Unset upgrades available so they will be recalculated
 */
void Packages::unset_upgrades_available()
{
	_upgrades_available = DONT_KNOW;
}

/**
 * Check if there are any upgrades available for installed packages
 */
bool Packages::upgrades_available()
{
	if (_upgrades_available == DONT_KNOW)
	{
		_upgrades_available = NO;

		const pkg::binary_control_table& ctrltab = _package_base->control();
		std::string prev_pkgname;

		for (pkg::binary_control_table::const_iterator i=ctrltab.begin();
		 i !=ctrltab.end(); ++i)
		{
		   std::string pkgname=i->first.pkgname;
		   if (pkgname!=prev_pkgname)
		   {
			  // Don't use i->second for ctrl as it may not be the latest version
			  // instead look it up.
			  prev_pkgname=pkgname;

			  pkg::status curstat=_package_base->curstat()[pkgname];
			  if (curstat.state()>=pkg::status::state_installed)
			  {
			 	  const pkg::control& ctrl=_package_base->control()[pkgname];
				  pkg::version inst_version(curstat.version());
				  pkg::version cur_version(ctrl.version());

				  if (inst_version < cur_version)
				  {
					  _upgrades_available = YES;
					  break; // Don't need to check any more
				  }
			  }
		   }
		}
	}

	return (_upgrades_available != NO);
}



/**
 * Convert paths to path relative to Boot$Dir if possible
 *
 * @param full_path path to convert
 * @returns path definition relative to boot if possible
 *          or original path if not.
 */
std::string Packages::make_path_definition(const std::string &full_path)
{
	std::string result(full_path);

	if (tbx::find_ignore_case(full_path, ".!BOOT.") == std::string::npos)
	{
		const char *boot_path = getenv("Boot$Dir");
		if (boot_path != 0 && strlen(boot_path) < result.size())
		{
			const char *parent_end = strrchr(boot_path, '.');
			if (parent_end != 0)
			{
				int match = 0;

				while (boot_path < parent_end && tolower(*boot_path) == tolower(result[match]))
				{
					boot_path++;
					match++;
				}
				if (boot_path == parent_end && result[match] == '.')
				{
					result.replace(0, match, "<Boot$Dir>.^");
				}
			}
		}
	}

	return result;
}

/**
 * Start a new log and return it
 */
std::tr1::shared_ptr<pkg::log> Packages::new_log()
{
    _log = std::tr1::shared_ptr<pkg::log>(new pkg::log());
    return _log;
}
