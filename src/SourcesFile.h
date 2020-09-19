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
 * SourceFile.h
 *
 *  Created on: 20 Nov 2015
 *      Author: alanb
 */

#ifndef SOURCEFILE_H_
#define SOURCEFILE_H_

#include <string>
#include <vector>
#include <utility>

/**
 * Class to read and write package sources with enabled information.
 *
 * Remarks the standard libpkg sources discards the enabled information
 * so use this class when updating the file itself
 */
class SourcesFile
{
public:
	typedef std::pair<std::string, bool> Source;
private:
	std::vector<Source> _sources;

public:
	SourcesFile();
	virtual ~SourcesFile();

	void load();
	void save();

	const std::vector<Source> &sources() const {return _sources;}

	void add(const std::string &url, bool enabled);
	void erase(int index) {_sources.erase(_sources.begin() + index);}
	int find(const std::string &url) const;
	const std::string &url(int index) const {return _sources[index].first;}
	void url(int index, const std::string &url);
	void enable(int index, bool enable) {_sources[index].second = enable;}
	bool enabled(int index) const {return _sources[index].second;}
};

#endif /* SOURCEFILE_H_ */
