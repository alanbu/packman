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
 * MainMenu.cc
 *
 *  Created on: 23-Jul-2009
 *      Author: alanb
 */

#include "MainMenu.h"
#include "MainWindow.h"

#include "libpkg/binary_control.h"

#include "tbx/objectdelete.h"

MainMenu::MainMenu(tbx::Object object) :
	_menu(object),
	_package_item(_menu.item(1))
{

	_menu.add_about_to_be_shown_listener(this);

	// Ensure class is deleted if object is
	_menu.add_object_deleted_listener(new tbx::ObjectDeleteClass<MainMenu>(this));
}

MainMenu::~MainMenu()
{
}

/**
 * Find currently selected package and update menu
 */
void MainMenu::about_to_be_shown(tbx::AboutToBeShownEvent &event)
{
	MainWindow *main = MainWindow::from_window(event.id_block().ancestor_object());
	const pkg::binary_control *ctrl = main->selected_package();

	if (ctrl == 0)
	{
		_package_item.text("Package ''");
		_package_item.fade(true);
	} else
	{
		_package_item.text("Package '" + ctrl->pkgname() + "'");
		_package_item.fade(false);
	}
}
