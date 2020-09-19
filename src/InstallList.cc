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
/* InstallList.cc
 *
 *  Created on: 7 May 2015
 *      Author: alanb
 */

#include "InstallList.h"
#include "Packages.h"
#include "PackageFilter.h"

#include "libpkg/pkgbase.h"
#include "libpkg/component.h"
#include "libpkg/boot_options_file.h"


#include "tbx/tag.h"
#include "tbx/stringutils.h"
#include "tbx/path.h"

#include <fstream>
#include <algorithm>

/**
 * Constructor for empty list
 *
 * use load method to load it from a file
 */
InstallList::InstallList()
{

}

/**
 * Constructor
 *
 * @param include_sources true if the list of used sources should be included
 * @param include_path true if the list of paths should be included
 */
InstallList::InstallList(bool include_sources, bool include_paths)
{
	pkg::pkgbase *package_base = Packages::instance()->package_base();
	pkg::path_table &paths = package_base->paths();

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
		// List of paths that are computer specific so they should be excluded
		const char *exclude[] = {"Boot","Bootloader","Resources", "RiscPkg",
				"Sprites","SysVars", "System","ToBeLoaded","ToBeTasks",0};

		for (auto path : paths)
		{
			const char **check = exclude;
			bool ok = true;
			while (ok && *check)
			{
				if (path.first == *check) ok = false;
				check++;
			}
			if (ok) _paths.push_back(path);
		}
	}

    const std::vector<PackageKey> &package_list = Packages::instance()->package_list();
	const pkg::binary_control_table& ctrltab = package_base->control();
	pkg::look_at_options look_at;
	pkg::run_options run;
	pkg::add_to_apps_options add_to_apps;

	for (auto &pkgkey : package_list)
	{
		const pkg::binary_control &ctrl = ctrltab[pkgkey];
		pkg::status_table::const_iterator sti = package_base->curstat().find(pkgkey.pkgname);

		if (sti != package_base->curstat().end()
				 && (*sti).second.state() == pkg::status::state_installed)
		{
			PackageInfo info;
			info.name = pkgkey.pkgname;
			info.version = ctrl.version();
			info.flags = (*sti).second.flags();

			std::string comp_str = ctrl.components();
			if (!comp_str.empty())
			{
				try
				{
					std::vector<pkg::component> comps;
					pkg::parse_component_list(comp_str.begin(), comp_str.end(), &comps);
					for (auto i = comps.begin(); i !=comps.end(); ++i)
					{
						if (i->flag(pkg::component::movable))
						{
						   std::string path = paths(i->name(), pkgkey.pkgname);

						   ComponentInfo comp_info;
						   comp_info.logical_path = i->name();
						   comp_info.flags = 0;
						   if (look_at.contains(path)) comp_info.flags |= 1 << pkg::component::look_at;
						   if (run.contains(path)) comp_info.flags |= 1 << pkg::component::run;
						   if (add_to_apps.contains(path)) comp_info.flags |= 1 << pkg::component::add_to_apps;
						   info.components.push_back(comp_info);
						}
					}
				} catch(...)
				{
					// Ignore exceptions and don't add components
				}
			}

			_packages.push_back(info);
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
	tbx::TagDoc doc;

	tbx::Tag *root_tag = doc.add_child("packagelist");
	root_tag->attribute("version", "1");

	if (!_sources.empty())
	{
		tbx::Tag *sources_tag = root_tag->add_child("sources");
		int source_id = doc.tag_id("source");
		int source_url_id = doc.attribute_id("url");
		for (auto source : _sources)
		{
			tbx::Tag *source_tag = sources_tag->add_child(source_id);
			source_tag->attribute(source_url_id, source);
		}
	}
	if (!_paths.empty())
	{
		tbx::Tag *paths_tag = root_tag->add_child("paths");
		int path_id = doc.tag_id("path");
		int logical_id = doc.attribute_id("logical");
		int physical_id = doc.attribute_id("physical");
		for (auto path : _paths)
		{
			tbx::Tag *path_tag = paths_tag->add_child(path_id);
			path_tag->attribute(physical_id, path.second);
			path_tag->attribute(logical_id, path.first);
		}
	}

	tbx::Tag *packages_tag = root_tag->add_child("packages");
	int package_id = doc.tag_id("package");
	int components_id = doc.tag_id("components");
	int component_id = doc.tag_id("component");
	int name_id = doc.attribute_id("name");
	int version_id = doc.attribute_id("version");
	int auto_id = doc.attribute_id("auto");
	int hold_id = doc.attribute_id("hold");
	int look_at_id = doc.attribute_id("look_at");
	int run_id = doc.attribute_id("run");
	int add_to_apps_id  = doc.attribute_id("add_to_apps");
	for (auto package : _packages)
	{
		tbx::Tag *package_tag = packages_tag->add_child(package_id);
		if (package.flags & (1<<pkg::status::flag_auto)) package_tag->attribute(auto_id);
		if (package.flags & (1<<pkg::status::flag_hold)) package_tag->attribute(hold_id);
		package_tag->attribute(version_id, package.version);
		package_tag->attribute(name_id,  package.name);
		if (!package.components.empty())
		{
			tbx::Tag *comps_tag = package_tag->add_child(components_id);
			for (auto comp : package.components)
			{
				tbx::Tag *comp_tag = comps_tag->add_child(component_id);
				if (comp.flags & (1<<pkg::component::look_at)) comp_tag->attribute(look_at_id);
				if (comp.flags & (1<<pkg::component::run)) comp_tag->attribute(run_id);
				if (comp.flags & (1<<pkg::component::add_to_apps)) comp_tag->attribute(add_to_apps_id);
				comp_tag->attribute(name_id, comp.logical_path);
			}
		}
	}

	std::ofstream f(filename);
	if (!f) return;
	f << doc;
	f.close();
}

/**
 * Load install list
 *
 * @param filename package list file name
 * @throws run item exception on failure
 */
void InstallList::load(const std::string &filename)
{
	std::ifstream in(filename);
	if (!in) throw std::runtime_error("Unable to open file for reading");

	tbx::TagDoc doc;
	in >> doc;
	in.close();

	tbx::Tag *root_tag = doc.first_child();

	tbx::Tag *sources_tag = root_tag->find_child("sources");
	if (sources_tag)
	{
		int source_url_id = doc.attribute_id("url");
		tbx::Tag *source_tag = sources_tag->first_child();
		while (source_tag)
		{
			std::string url = source_tag->attribute_value(source_url_id);
			_sources.push_back(url);
			source_tag = source_tag->next();
		}
	}

	tbx::Tag *paths_tag = root_tag->find_child("paths");
	if (paths_tag)
	{
		tbx::Tag *path_tag = paths_tag->first_child();
		int logical_id = doc.attribute_id("logical");
		int physical_id = doc.attribute_id("physical");
		while (path_tag)
		{
			std::string logical = path_tag->attribute_value(logical_id);
			std::string physical = path_tag->attribute_value(physical_id);
			_paths.push_back(std::make_pair(logical, physical));
			path_tag = path_tag->next();
		}
	}

	tbx::Tag *packages_tag = root_tag->find_child("packages");
	if (packages_tag)
	{
		tbx::Tag *package_tag = packages_tag->first_child();
		int name_id = doc.attribute_id("name");
		int version_id = doc.attribute_id("version");
		int auto_id = doc.attribute_id("auto");
		int hold_id = doc.attribute_id("hold");
		while (package_tag)
		{
			PackageInfo info;
			info.name = package_tag->attribute_value(name_id);
			info.version = package_tag->attribute_value(version_id);
			info.flags = 0;
			if (package_tag->find_attribute(auto_id) != nullptr) info.flags |= (1<<pkg::status::flag_auto);
			if (package_tag->find_attribute(hold_id) != nullptr) info.flags |= (1<<pkg::status::flag_hold);

			_packages.push_back(info);
			package_tag = package_tag->next();
		}
	}
}

/**
 * Update list to take into accounts currents package state
 *
 * Remove entries that already exists on this machine
 * Removes auto-installed packages as they will be added depending
 * on the main packages.
 * Moves package that are not in the current package list to the
 * unavailable_packages list.
 */
void InstallList::remove_existing()
{
	pkg::pkgbase *package_base = Packages::instance()->package_base();
	if (!_sources.empty())
	{
		pkg::source_table &source_table = package_base->sources();

		_sources.erase(std::remove_if(_sources.begin(), _sources.end(),
				[&source_table](const std::string &source)
				{
			       return (std::find(source_table.begin(), source_table.end(), source) != source_table.end());
				}
				),
				_sources.end());
	}

	if (!_paths.empty())
	{
		std::vector<PathInfo> new_paths;
		pkg::path_table &path_table = package_base->paths();
		for( auto path : _paths)
		{
			auto found = path_table.find(path.first);
			if (found == path_table.end())
			{
				new_paths.push_back(path);
			} else
			{
				if (!tbx::equals_ignore_case(path.second, found->second))
				{
					_clash_paths.push_back(path);
				}
			}
		}
		_paths = new_paths;
	}

	if (!_packages.empty())
	{
		const pkg::binary_control_table& ctrltab = package_base->control();
		std::vector<PackageInfo> check;
		check.swap(_packages);
		InstalledFilter filter;

		for (auto pack : check)
		{
			const pkg::binary_control &ctrl = ctrltab[pack.name];
			if (!(pack.flags & (1<<pkg::status::flag_auto)))
			{
				if (ctrl.pkgname() != pack.name)
				{
					// Not found
					_unavailable_packages.push_back(pack);
				} else if (!filter.ok_to_show(ctrl))
				{
					_packages.push_back(pack);
				}
			}
		}
	}
}

/**
 * Get a list of unique drives for the paths to be added
 *
 * @param set of unique drives to be added
 */
void InstallList::get_path_drives(std::set<std::string> &drives)
{
	for (auto path : _paths)
	{
		std::string cpath(path.second);
		std::string::size_type first_dot = cpath.find('.');
		if (first_dot != std::string::npos)
		{
			drives.insert(cpath.substr(0, first_dot));
		}
	}
}

/**
 * Remap the drives in the paths using the given mapping
 *
 * @param mapping of old drive path to new
 */
void InstallList::remap_paths(std::vector<std::pair<std::string,std::string>> &mapping)
{
	for (auto &path : _paths)
	{
		std::string::size_type first_dot = path.second.find('.');
		if (first_dot != std::string::npos)
		{
			std::string old_drive(path.second.substr(0, first_dot));

			for (auto drive : mapping)
			{
				if (drive.first == old_drive)
				{
					path.second = drive.second + path.second.substr(first_dot);
				}
			}
		}
	}
}

/**
 * Recheck the unavailable packages list
 *
 * Checks if any of the unavailable packages are now available and moves
 * them to the packages list if they are
 */
void InstallList::recheck_unavailable()
{
	pkg::pkgbase *package_base = Packages::instance()->package_base();
	const pkg::binary_control_table& ctrltab = package_base->control();

	unsigned int i = 0;
	while (i < _unavailable_packages.size())
	{
		PackageInfo &pack = _unavailable_packages[i];
		const pkg::binary_control &ctrl = ctrltab[pack.name];
		if (ctrl.pkgname() == pack.name)
		{
			// Now on list
			_packages.push_back(pack);
			_unavailable_packages.erase(_unavailable_packages.begin() + i);
		} else
		{
			i++;
		}
	}
}
