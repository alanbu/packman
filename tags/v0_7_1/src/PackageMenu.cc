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
 * PackageMenu.cc
 *
 *  Created on: 20-Aug-2009
 *      Author: alanb
 */

#include "PackageMenu.h"
#include "MainWindow.h"
#include "Packages.h"

#include "libpkg/binary_control.h"
#include "libpkg/pkgbase.h"


#include "tbx/objectdelete.h"

PackageMenu::PackageMenu(tbx::Object object) :
	_menu(object),
	_install(_menu.item(1))
{
	_installed_items[0] = _menu.item(2); // Remove
	_installed_items[1] = _menu.item(3); // apps
	_installed_items[2] = _menu.item(5); // copyright

	_menu.add_about_to_be_shown_listener(this);
	_menu.add_object_deleted_listener(new tbx::ObjectDeleteClass<PackageMenu>(this));
}

PackageMenu::~PackageMenu()
{
}

/**
 * Find currently selected package and update menu
 */
void PackageMenu::about_to_be_shown(tbx::AboutToBeShownEvent &event)
{
	MainWindow *main = MainWindow::from_window(event.id_block().ancestor_object());
	const pkg::binary_control *ctrl = main->selected_package();

	if (ctrl == 0) return; // Shouldn't happen

	MainWindow::InstallState state = main->install_state(ctrl);

	_install.fade(state == MainWindow::INSTALLED);
	bool not_installed = (state == MainWindow::NOT_INSTALLED);

	for (int j = 0; j < _num_installed_items; j++)
		_installed_items[j].fade(not_installed);
}
