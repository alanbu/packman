/*********************************************************************
* This file is part of PkgTest (PackMan unit testing)
*
* Copyright 2014-2020 AlanBuckley
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
