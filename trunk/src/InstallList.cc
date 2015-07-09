/*********************************************************************
* Copyright 2015 Alan Buckley
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
/* InstallList.cc
 *
 *  Created on: 7 May 2015
 *      Author: alanb
 */

#include "InstallList.h"
#include "Packages.h"
#include "PackageFilter.h"
#include "libpkg/pkgbase.h"
#include <fstream>

/**
 * Constructor
 *
 * @param include_sources true if the list of used sources should be included
 * @param include_path true if the list of paths should be included
 */
InstallList::InstallList(bool include_sources, bool include_paths)
{
	pkg::pkgbase *package_base = Packages::instance()->package_base();
	if (include_sources)
	{
		pkg::source_table &sources = package_base->sources();
		for(auto source : sources)
		{
			_sources.push_back(source);
		}
	}
	if (include_paths)
	{
		pkg::path_table &paths = package_base->paths();
		for (auto path : paths)
		{
			_paths.push_back(path);
		}
	}

    const std::vector<std::string> &package_list = Packages::instance()->package_list();
	const pkg::binary_control_table& ctrltab = package_base->control();

	InstalledFilter filter;
	for (auto pkgname : package_list)
	{
		const pkg::binary_control &ctrl = ctrltab[pkgname];
		if (filter.ok_to_show(ctrl))
		{
			_packages.push_back(std::pair<std::string,std::string>(pkgname, ctrl.version()));
			//TODO: Move auto installed to different list
		}
	}
}

InstallList::~InstallList()
{
}

/**
 * Save install list to the given file
 */
void InstallList::save(const std::string &filename)
{
	std::ofstream f(filename);
	if (!f) return;

	f << "<packagelist version = \"1\">" << std::endl;
	if (!_sources.empty())
	{
		f << "  <sources>" << std::endl;
		for (auto source : _sources)
		{
			f << "    <source path = \"" << source << "\"/>" << std::endl;
		}
		f << "  </sources>" << std::endl;
	}
	if (!_paths.empty())
	{
		f << "  <paths>" << std::endl;
		for (auto path : _paths)
		{
			f << "    <path logical = \"" << path.first
			  << "\" physical = \"" << path.second << "\"/>"
			  << std::endl;
		}
	}

	f << "  <packages>" << std::endl;
	for (auto package : _packages)
	{
		f << "    <package name = \"" << package.first
	      << "\" version = \"" << package.second << "\"/>"
	      << std::endl;
	}

	f << "</packagelist>";
	f.close();
}
