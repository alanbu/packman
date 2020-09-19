/*********************************************************************
* This file is part of PackMan
*
* Copyright 2009-2020 AlanBuckley
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
 * PackageFilter.h
 *
 *  Created on: 18-Jun-2009
 *      Author: alanb
 */

#ifndef PACKAGEFILTER_H_
#define PACKAGEFILTER_H_

#include "libpkg/binary_control.h"
#include <set>

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
 * Filter to show new packages
 */
class WhatsNewFilter : public PackageFilter
{
private:
	std::set<std::string> _whats_new;

public:
	WhatsNewFilter();
	virtual ~WhatsNewFilter() {}

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
