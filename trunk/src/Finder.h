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
/*
 * Finder.h
 *
 *  Created on: 11 May 2015
 *      Author: alanb
 */

#ifndef FINDER_H_
#define FINDER_H_

#include <string>
#include <vector>

/**
 * Class to find a component/installed file, report on it and
 * give options to replace components
 */
class Finder
{
public:
	/**
	 * Details on found results
	 */
	struct Result
	{
		Result() {}
		Result(const std::string &pkgname, const std::string &path) : package(pkgname), logical_path(path) {}
		std::string package;
		std::string logical_path;
	};

	typedef std::vector<Result> Results;
private:
	Results _results;

	// Work variables
	std::string _name;
	bool _exact;
	std::string _search_for;
	bool _has_dir;

public:
	Finder();
	virtual ~Finder();

	const Results &results() const {return _results;}

	bool find_component(const std::string &name, bool exact);
	bool find_installed_file(const std::string &name, bool exact);

private:
	void init_find(const std::string &name, bool exact);
	void check_path(const std::string &check, const std::string &pkgname);
};

#endif /* FINDER_H_ */
