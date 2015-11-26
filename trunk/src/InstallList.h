/*********************************************************************
* Copyright 2009-2015 Alan Buckley
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
 * InstallList.h
 *
 *  Created on: 7 May 2015
 *      Author: alanb
 */

#ifndef INSTALLLIST_H_
#define INSTALLLIST_H_

#include <string>
#include <vector>
#include <set>
#include "libpkg/status.h"

/**
 * Manage list of installed packages optionally with the sources
 * and paths
 */
class InstallList
{
public:
	struct ComponentInfo
	{
		std::string logical_path;
		unsigned int flags;
	};
	struct PackageInfo
	{
		std::string name;
		std::string version;
		unsigned int flags;
		std::vector<ComponentInfo> components;
	};
	typedef std::pair<std::string, std::string> PathInfo;
private:
	std::vector<PackageInfo> _packages;
	std::vector<PathInfo> _paths;
	std::vector<std::string> _sources;

	std::vector<PackageInfo> _unavailable_packages;
	std::vector<PathInfo> _clash_paths;

public:
	InstallList();
	InstallList(bool include_sources, bool include_paths);
	virtual ~InstallList();

	int packages_count() const {return _packages.size();}
	int paths_count() const {return _paths.size();}
	int sources_count() const {return _sources.size();}

	const std::vector<PackageInfo> &packages() const {return _packages;}
	const std::vector<PathInfo> &paths() const {return _paths;}
	const std::vector<std::string> &sources() const {return _sources;}

	const std::vector<PackageInfo> &unavailable_packages() const {return _unavailable_packages;}
	const std::vector<PathInfo> &clash_paths() const {return _clash_paths;}

	void save(const std::string &filename);
	void load(const std::string &filename);

	void remove_existing();
	void get_path_drives(std::set<std::string> &drives);
	void remap_paths(std::vector<std::pair<std::string,std::string>> &mapping);
	void recheck_unavailable();
};

#endif /* INSTALLLIST_H_ */
