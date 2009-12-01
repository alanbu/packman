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
 * InstallWindow.cc
 *
 *  Created on: 25-Mar-2009
 *      Author: alanb
 */

#include "IRWindow.h"
#include "Packages.h"
#include "Commands.h"

#include "libpkg/pkgbase.h"

#include <set>
#include <sstream>

IRWindow::IRWindow(bool remove) : _window((remove) ? "Remove" : "Install"),
    _remove(remove),
	_package (_window.gadget(1)),
	_version (_window.gadget(3)),
	_summary (_window.gadget(5)),
	_auto_remove(_window.gadget(14)),
	_remove_number(_window.gadget(17)),
	_install_button (_window.gadget(12))
{
	if (remove)
	{
		_install_button.text("Remove");
	} else
	{
		_install_button.text("Install");
		_install_depends =_window.gadget(7);
		_install_number =_window.gadget(9);
		_install_size = _window.gadget(11);
	}
	_install_button.add_select_command(new CommitCommand());
}

IRWindow::~IRWindow()
{
}

/**
 * Show the dialog
 */
void IRWindow::show()
{
	_window.show_as_menu();
}

/**
 * Set package to install
 */
void IRWindow::set_package(const pkg::binary_control *bctrl)
{
    pkg::pkgbase * package_base = Packages::instance()->package_base ();
    std::string pkgname(bctrl->pkgname());

	_package.text(pkgname);
	_version.text (bctrl->version ());
	_summary.text (bctrl->short_description ().substr (0, 127));

	pkg::status_table & seltable = package_base->selstat ();

	pkg::status_table::const_iterator i;
	std::set < std::string > seed;

	// Clear any old selection
	for (i = seltable.begin(); i != seltable.end(); ++i)
	{
	 pkg::status curstat = package_base->curstat ()[i->first];
	 if (curstat != i->second)
	 {
		seltable.insert(i->first, curstat);
		seed.insert(i->first);
	 }
	}

	package_base->fix_dependencies(seed);
	package_base->remove_auto();

	seed.clear();

	// Figure out dependencies
	seed.insert(pkgname);

	pkg::status selstat = seltable[pkgname];
	if (_remove) selstat.state (pkg::status::state_removed);
	else selstat.state (pkg::status::state_installed);

	const pkg::control& ctrl=package_base->control()[pkgname];
	selstat.version(ctrl.version());

	seltable.insert (pkgname, selstat);
	package_base->fix_dependencies (seed);
	package_base->remove_auto ();

	unsigned int num = 0;
	unsigned int remove = 0;
	unsigned long long total_size = 0;

	if (!_remove)
	{
		// Extra details for install
		for (pkg::status_table::const_iterator i = seltable.begin ();
		i != seltable.end (); ++i)
		{
		 pkg::status curstat = package_base->curstat ()[i->first];
		 if (pkg::unpack_req (curstat, i->second))
		 {
		   if (i->first != pkgname)
		   {
			 _install_depends.add_item (i->first);
		   }
		   num++;
		   total_size += download_size(i->first);
		 }
		}

		std::ostringstream ons;
		ons << num;
		_install_number.text (ons.str ());

		std::string unit[] = {"B", "KB", "MB", "GB", "TB"};
		int units = total_size;
		int unittype = 0;
		while ((units >= 4096) && (unittype < 4))
		{
		 units >>= 10;
		 ++unittype;
		}

		std::ostringstream oss;
		oss << units << " " << unit[unittype];
		_install_size.text (oss.str ());
	}

	// Count packages to remove.
	for (std::map<std::string,pkg::status>::const_iterator i=package_base->curstat().begin();
		i!=package_base->curstat().end();++i)
	{
		const std::string& pkgname2=i->first;
		const pkg::status& curstat=i->second;
		const pkg::status& selstat=package_base->selstat()[pkgname2];
		if (remove_req(curstat,selstat))
		{
			if (pkgname2 != pkgname)
			{
				_auto_remove.add_item(pkgname2);
			}
			++remove;
		}
	}

	std::ostringstream ors;
	ors << remove;
	_remove_number.text (ors.str ());
}

/**
* Get the download size of a package
*/
unsigned int IRWindow::download_size(std::string pkgname)
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
