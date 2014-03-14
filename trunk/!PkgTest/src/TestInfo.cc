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
 * TestInfo.cc
 *
 *  Created on: 11 Feb 2014
 *      Author: alanb
 */

#include "TestInfo.h"
#include "tbx/stringutils.h"
#include <fstream>

/**
 * Construct a test list and load from a file
 */
TestList::TestList(const std::string &file_name)
{
	read(file_name);
}

/**
 * Load test list from a file
 */
void TestList::read(const std::string &file_name)
{
	std::ifstream in(file_name.c_str());
	in.peek();
	while (in && !in.eof())
	{
		TestInfo info;
		in >> info.script;
		in.get(); // Skip tab
		std::getline(in, info.description);
		push_back(info);
		in.peek();
	}
}

/**
 * Save test list to a file
 */
void TestList::write(const std::string &file_name)
{
	std::ofstream out(file_name.c_str());
	for(iterator i = begin(); i != end(); ++i)
	{
		out << (*i).script << "\t" << (*i).description << std::endl;
	}
}


/**
 * Check if test list contains a given script
 */
bool TestList::contains(const std::string &script)
{
	for(iterator i = begin(); i != end(); ++i)
	{
		if (tbx::equals_ignore_case(i->script,script)) return true;
	}

	return false;
}
