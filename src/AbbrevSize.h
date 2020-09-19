/*********************************************************************
* This file is part of PackMan
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
 * AbbrevSize.h
 *
 *  Created on: 30 Sep 2011
 *      Author: alanb
 */

#ifndef ABBREVSIZE_H_
#define ABBREVSIZE_H_

#include <sstream>

/**
 * Abbreviate a number of bytes
 */
inline std::string abbrev_size(long long size)
{
	std::string unit[] = {"B", "KB", "MB", "GB", "TB"};
	long long units = size;
	int unittype = 0;
	while ((units >= 4096) && (unittype < 4))
	{
	 units >>= 10;
	 ++unittype;
	}

	std::ostringstream oss;
	oss << units << " " << unit[unittype];

	return oss.str();
}


#endif /* ABBREVSIZE_H_ */
