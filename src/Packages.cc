/*********************************************************************
* Copyright 2009 Alan Buckley
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
#include <string>
#include <set>

#include "swis.h"
#include "tbx/swixcheck.h"
#include "tbx/stringutils.h"

using namespace std;

Packages *Packages::_instance = 0;

Packages g_packages; // Single instance of the packages

Packages::Packages() : _package_base(0), _upgrades_available(DONT_KNOW)
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
		} catch(tbx::OsError &err)
		{
			std::string msg(err.what());
			new ErrorWindow(msg, "OS Error when trying to load/initialise packages");
			delete _package_base;
			_package_base=0;
		} catch(std::runtime_error &rerr)
		{
			std::string msg(rerr.what());
			new ErrorWindow(msg, "Error when trying to load/initialise packages");
			delete _package_base;
			_package_base=0;
		} catch (...)
		{
			new ErrorWindow("Could not load/initialise package system", "Error when trying to load/initialise packages");
			delete _package_base;
			_package_base=0;
		}
	}
	return (_package_base != 0);
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

	// Get list of packages with there stateClear any old selection
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
		const char *boot_path = getenv("<Boot$Dir>");
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
				if (boot_path == parent_end && result[match+1] == '.')
				{
					result.replace(0, match, "<Boot$Dir>.^");
				}
			}
		}
	}

	return result;
}
