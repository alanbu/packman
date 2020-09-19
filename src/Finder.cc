/*********************************************************************
* This file is part of PackMan
*
* Copyright 2015-2020 AlanBuckley
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
/*
 *  * Finder.cc
 *
 *  Created on: 11 May 2015
 *      Author: alanb
 */

#include "Finder.h"

#include "Packages.h"
#include "libpkg/pkgbase.h"
#include "libpkg/component.h"
#include "tbx/stringutils.h"
#include "tbx/messagewindow.h"

#include <fstream>

Finder::Finder() : _exact(false), _has_dir(false)
{
}

Finder::~Finder()
{
}

/**
 * Find component in list of packages
 *
 * @param name - leaf name of the component
 * @param exact - true for an exact match
 * @returns true if we have a match
 */
bool Finder::find_component(const std::string &name, bool exact)
{
	_results.clear();

	Packages *packages = Packages::instance();
	if (packages == 0) return false; // Safety check
	pkg::pkgbase *pb = Packages::instance()->package_base();
	pkg::binary_control_table table = pb->control();

	init_find(name, exact);

	for(auto pkgkey : packages->package_list())
	{
		const pkg::binary_control &ctrl = table[pkgkey];
		if (!ctrl.components().empty())
		{
			std::string comp_text = ctrl.components();
			std::vector<pkg::component> comps;
			try
			{
				pkg::parse_component_list(comp_text.begin(), comp_text.end(), &comps);
			} catch(std::exception &e)
			{
				std::string msg("Unable to parse components in package ");
				msg += pkgkey.pkgname;
				msg += "\ncomponents = ";
				msg += ctrl.components();
				msg += "\nerror = ";
				msg += e.what();
				tbx::show_message(msg);
			}
			for(auto comp : comps)
			{
				check_path(comp.name(), pkgkey.pkgname);
			}
		}
	}

	return !_results.empty();
}

/*
* @param name - leaf name of the installed file to find
* @param exact - true for an exact match
* @returns true if we have a match
*/
bool Finder::find_installed_file(const std::string &name, bool exact)
{
	_results.clear();
	Packages *packages = Packages::instance();
	if (packages == 0) return false; // Safety check
	pkg::pkgbase *pb = Packages::instance()->package_base();

	init_find(name, exact);

	for (auto pkgkey : packages->package_list() )
	{
		pkg::status_table::const_iterator sti = pb->curstat().find(pkgkey.pkgname);

		if (sti != pb->curstat().end()
				 && (*sti).second.state() == pkg::status::state_installed)
		{
			std::string files_pathname=pb->info_pathname(pkgkey.pkgname)+std::string(".Files");
			std::ifstream in(files_pathname.c_str());

			while (in)
			{
				// Read pathname from manifest.
				std::string logical_path;
				getline(in,logical_path);

				check_path(logical_path, pkgkey.pkgname);
			}
		}
	}

	return !_results.empty();
}

/**
 * Set up work fields for find
 *
 * @param name value to search for
 * @param exact true for an exact match
 */
void Finder::init_find(const std::string &name, bool exact)
{
	_name = name;
	_exact = exact;
	_has_dir = false;

	std::string::size_type leaf_pos = name.rfind(".");
	if (leaf_pos == std::string::npos)
	{
		_search_for = name;
	} else
	{
		_has_dir = true;
		_search_for = name.substr(leaf_pos+1);
	}
}

/**
 * Check if the given logical path matches the search.
 *
 * Updates results() if the path matches
 *
 * @param check logical path of item to check
 */
void Finder::check_path(const std::string &check, const std::string &pkgname)
{
	std::string::size_type leaf_pos = check.rfind(".");
	std::string leaf = (leaf_pos == std::string::npos) ?
			check
			: check.substr(leaf_pos+1);
	if (_exact)
	{
		if (tbx::equals_ignore_case(_search_for, leaf))
		{
			if (_has_dir)
			{
				std::string installed_path = Packages::instance()->package_base()->paths()(check, pkgname);
				if (tbx::equals_ignore_case(_name, installed_path))
				{
					_results.push_back(Result(pkgname, check));
				}
			} else
			{
				_results.push_back(Result(pkgname, check));
			}
		}
	} else if (tbx::find_ignore_case(leaf, _search_for) != std::string::npos)
	{
		_results.push_back(Result(pkgname, check));
	}
}

