/*********************************************************************
* Copyright 2010-2015 Alan Buckley
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
