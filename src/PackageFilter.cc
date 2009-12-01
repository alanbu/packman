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
 * PackageFilter.cc
 *
 *  Created on: 18-Jun-2009
 *      Author: alanb
 */

#include "PackageFilter.h"
#include "Packages.h"

#include "tbx/stringutils.h"

#include "libpkg/pkgbase.h"

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
		  pkg::version inst_version((*sti).second.version());
		  pkg::version cur_version(ctrl.version());
		  return (inst_version < cur_version);
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

