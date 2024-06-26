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
 * PackageFilter.cc
 *
 *  Created on: 18-Jun-2009
 *      Author: alanb
 */

#include "PackageFilter.h"
#include "Packages.h"

#include "tbx/stringutils.h"

#include "libpkg/pkgbase.h"

#include <iterator>
#include <algorithm>
#include <fstream>

/**
 * Check if a package is installed.
 *
 * @return true if package is installed
 */
bool InstalledFilter::ok_to_show(const pkg::binary_control &ctrl)
{
	std::string pkgname = ctrl.pkgname();

    pkg::pkgbase *package_base = Packages::instance()->package_base();
	pkg::status_table::const_iterator sti = package_base->curstat().find(pkgname);

	return (sti != package_base->curstat().end()
			 && (*sti).second.state() == pkg::status::state_installed);
}

/**
 * Check if a package had an upgrade.
 */
bool UpgradeFilter::ok_to_show(const pkg::binary_control &ctrl)
{
	std::string pkgname = ctrl.pkgname();

    pkg::pkgbase *package_base = Packages::instance()->package_base();
	pkg::status_table::const_iterator sti = package_base->curstat().find(pkgname);
	if (sti == package_base->curstat().end()
		  || (*sti).second.state() != pkg::status::state_installed
		  )
	{
		return false; // Not installed so can't have an upgrade
	} else
	{
		  pkg::env_packages_table::const_iterator epi = package_base->env_packages().find(pkgname);
		  if (epi == package_base->env_packages().end())
		  {
			  return false; // No better version for this environment
		  }

		  pkg::version inst_version((*sti).second.version());
		  return (inst_version < epi->second.pkgvrsn);
	}
}

/**
 * Show packages in the given section only
 */
bool SectionFilter::ok_to_show(const pkg::binary_control &ctrl)
{
	pkg::control::const_iterator s = ctrl.find(_section_key);

	return (s != ctrl.end() && (*s).second == _section);
}

/**
 * Read set of Whats new from file
 */
WhatsNewFilter::WhatsNewFilter()
{
	std::ifstream in("<Choices$Write>.PackMan.WhatsNew");
	if (in)
	{
		std::istream_iterator<std::string> in_iter(in);
		std::istream_iterator<std::string> eos;
		std::copy(in_iter, eos, std::inserter(_whats_new, _whats_new.end()));
	}
}

/**
 * Show the last new packages only
 */
bool WhatsNewFilter::ok_to_show(const pkg::binary_control &ctrl)
{
	if (_whats_new.empty()) return false;
	std::set<std::string>::iterator found = _whats_new.find(ctrl.pkgname());
	return (found != _whats_new.end());
}


/**
 * Construct search filter
 *
 * Note: search_in pointer will be owned by this filter and deleted
 * when it is.
 *
 * @param text text to search for
 * @param filter to search_in or 0 for all packages.
 */
SearchFilter::SearchFilter(const std::string &text, PackageFilter *search_in)
  : _text(text), _search_in(search_in)
{
}

SearchFilter::~SearchFilter()
{
	delete _search_in;
}

/**
 * Check if package contains text in name or description
 * within the given filter
 */
bool SearchFilter::ok_to_show(const pkg::binary_control &ctrl)
{
	// Discard if parent filter doesn't accept it
	if (_search_in && !_search_in->ok_to_show(ctrl)) return false;

	// First try name
	std::string::size_type pos = tbx::find_ignore_case(ctrl.pkgname(), _text);
	if (pos == std::string::npos)
	{
		// Then description
		pos = tbx::find_ignore_case(ctrl.description(), _text);
	}

	return (pos != std::string::npos);
}

/**
 * Show packages not in a given section
 */
bool ExcludeFilter::ok_to_show(const pkg::binary_control &ctrl)
{
	pkg::control::const_iterator s = ctrl.find(_section_key);

	return (s == ctrl.end() || (*s).second != _section);
}
