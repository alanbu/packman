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
