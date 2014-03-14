/*********************************************************************
* Copyright 2014 Alan Buckley
*
* This file is part of PkgTest (PackMan unit testing)
*
* PackMan is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* PkgTest is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with PackMan. If not, see <http://www.gnu.org/licenses/>.
*
*****************************************************************************/
/*
 * TestInfo.h
 *
 *  Created on: 11 Feb 2014
 *      Author: alanb
 */

#ifndef TESTINFO_H_
#define TESTINFO_H_

#include <string>
#include <vector>

/**
 * Structure to hold information on a single test
 */
struct TestInfo
{
	std::string script;
	std::string description;
};

/**
 * Class to hold the list of tests
 */
class TestList : public std::vector<TestInfo>
{
public:
	TestList() {}
	TestList(const std::string &file_name);
	void read(const std::string &file_name);
	void write(const std::string &file_name);

	bool contains(const std::string &script);

	typedef std::vector<TestInfo>::iterator iterator;
	typedef std::vector<TestInfo>::const_iterator const_iterator;
};

#endif /* TESTINFO_H_ */
