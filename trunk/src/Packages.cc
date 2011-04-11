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

using namespace std;

Packages *Packages::_instance = 0;

Packages g_packages; // Single instance of the packages

Packages::Packages() : _package_base(0)
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
				// Attempt to access package database.
				_package_base=new pkg::pkgbase("<Packages$Dir>","<PackMan$Dir>.Resources",
					"Choices:PackMan");
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
