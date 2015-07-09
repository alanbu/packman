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

/**
 * Manage list of installed packages optionally with the sources
 * and paths
 */
class InstallList
{
private:
	std::vector<std::pair<std::string, std::string> > _packages;
	std::vector<std::pair<std::string, std::string> > _paths;
	std::vector<std::string> _sources;

public:
	InstallList(bool include_sources, bool include_paths);
	virtual ~InstallList();

	void save(const std::string &filename);
};

#endif /* INSTALLLIST_H_ */
