/*********************************************************************
* Copyright 2009 Alan Buckley
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
 * PackageFilter.h
 *
 *  Created on: 18-Jun-2009
 *      Author: alanb
 */

#ifndef PACKAGEFILTER_H_
#define PACKAGEFILTER_H_

#include "libpkg/binary_control.h"

/**
 * Interface to filter the main view
 */
class PackageFilter {
public:
	PackageFilter() {}
	virtual ~PackageFilter() {}

	virtual bool ok_to_show(const pkg::binary_control &ctrl) = 0;
};

/**
 * Filter to show installed only packages
 */
class InstalledFilter : public PackageFilter
{
public:
	InstalledFilter() {}
	virtual ~InstalledFilter() {}

	virtual bool ok_to_show(const pkg::binary_control &ctrl);
};

/**
 * Filter to show packages with upgrades only
 */
class UpgradeFilter : public PackageFilter
{
public:
	UpgradeFilter() {}
	virtual ~UpgradeFilter() {}

	virtual bool ok_to_show(const pkg::binary_control &ctrl);
};

/**
 * Filter to show packages in a particular section
 */
class SectionFilter : public PackageFilter
{
private:
	pkg::control::key_type _section_key;
	std::string _section;

public:
	SectionFilter(const std::string &section) :
		_section_key("Section"), _section(section) {}
	virtual ~SectionFilter() {}

	virtual bool ok_to_show(const pkg::binary_control &ctrl);
};

/**
 * Filter to search packages
 */
class SearchFilter : public PackageFilter
{
private:
	std::string _text;
	PackageFilter *_search_in;

public:
	SearchFilter(const std::string &text, PackageFilter *search_in);
	virtual ~SearchFilter();

	virtual bool ok_to_show(const pkg::binary_control &ctrl);
};

#endif /* PACKAGEFILTER_H_ */
