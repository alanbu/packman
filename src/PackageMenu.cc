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
	_installed_items[2] = _menu.item(6); // Installed files

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
