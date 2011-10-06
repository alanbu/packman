/*********************************************************************
* Copyright 2011 Alan Buckley
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
 * UpgradeAllWindow.cc
 *
 *  Created on: 25-Mar-2009
 *      Author: alanb
 */

#include "UpgradeAllWindow.h"
#include "Packages.h"
#include "Commands.h"
#include "AbbrevSize.h"

#include "libpkg/pkgbase.h"

#include <set>
#include <sstream>

UpgradeAllWindow::UpgradeAllWindow() : _window("UpgradeAll"),
	_upgrades(_window.gadget(7)),
	_upgrades_number(_window.gadget(9)),
	_upgrades_size( _window.gadget(11)),
	_auto_remove(_window.gadget(14)),
	_remove_number(_window.gadget(17)),
	_install_button (_window.gadget(12))
{
	_install_button.add_select_command(new CommitCommand());
}

UpgradeAllWindow::~UpgradeAllWindow()
{
}

/**
 * Show the dialog */
void UpgradeAllWindow::show()
{
	_window.show_as_menu();
}

/**
 * Set all updates to install
 *
 * @returns true if there are any updates
 */
bool UpgradeAllWindow::set_upgrades()
{
    pkg::pkgbase * package_base = Packages::instance()->package_base();
	pkg::status_table & seltable = package_base->selstat ();
	std::set < std::string > seed;

	Packages::instance()->clear_selection();

    // Select all updates
    const pkg::binary_control_table& ctrltab = package_base->control();
    std::string prev_pkgname;

    for (pkg::binary_control_table::const_iterator i=ctrltab.begin();
	 i !=ctrltab.end(); ++i)
    {
	   std::string pkgname=i->first.pkgname;
	   if (pkgname!=prev_pkgname)
	   {
		  // Don't use i->second for ctrl as it may not be the latest version
		  // instead look it up.
		  prev_pkgname=pkgname;

          pkg::status curstat=package_base->curstat()[pkgname];
		  pkg::status selstat=package_base->selstat()[pkgname];
		  if (selstat.state()>=pkg::status::state_installed)
		  {
		 	 const pkg::control& ctrl=package_base->control()[pkgname];
			 selstat.version(ctrl.version());
			 package_base->selstat().insert(pkgname,selstat);
			 seed.insert(pkgname);
		  }
	   }
    }

	package_base->fix_dependencies (seed);
	package_base->remove_auto ();

	unsigned int num = 0;
	unsigned int remove = 0;
	unsigned long long total_size = 0;

	_upgrades.clear();

	std::set<std::string> upgrades;

	// Extra details for install
	for (pkg::status_table::const_iterator i = seltable.begin ();
		i != seltable.end (); ++i)
	{
		 pkg::status curstat = package_base->curstat ()[i->first];
		 if (pkg::unpack_req (curstat, i->second))
		 {
			_upgrades.add_item (i->first);
			upgrades.insert(i->first);
		    num++;
		    total_size += download_size(i->first);
		 }
	}

	std::ostringstream ons;
	ons << num;
	_upgrades_number.text (ons.str ());

	_upgrades_size.text (abbrev_size(total_size));

	_auto_remove.clear();

	// Count packages to remove.
	for (std::map<std::string,pkg::status>::const_iterator i=package_base->curstat().begin();
		i!=package_base->curstat().end();++i)
	{
		const std::string& pkgname2=i->first;
		const pkg::status& curstat=i->second;
		const pkg::status& selstat=package_base->selstat()[pkgname2];
		if (remove_req(curstat,selstat))
		{
			if (upgrades.find(pkgname2) == upgrades.end())
			{
				_auto_remove.add_item(pkgname2);
				++remove;
			}
		}
	}

	std::ostringstream ors;
	ors << remove;
	_remove_number.text (ors.str ());

    return (num > 0);
}

/**
* Get the download size of a package
*/
unsigned int UpgradeAllWindow::download_size(std::string pkgname)
{
	unsigned int size = 0;
    pkg::pkgbase * package_base = Packages::instance()->package_base ();
    const pkg::binary_control & ctrl = package_base->control ()[pkgname];

	pkg::control::const_iterator f = ctrl.find ("Size");
	if (f != ctrl.end ())
	{
	   std::istringstream in (f->second);
	   in >> size;
	}

	return size;
}
