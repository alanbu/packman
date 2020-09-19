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
