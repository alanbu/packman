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
 * MainMenu.cc
 *
 *  Created on: 23-Jul-2009
 *      Author: alanb
 */

#include "MainMenu.h"
#include "MainWindow.h"
#include "Packages.h"

#include "libpkg/binary_control.h"

#include "tbx/objectdelete.h"

MainMenu::MainMenu(tbx::Object object) :
	_menu(object),
	_package_item(_menu.item(1)),
	_upgrade_all_item(_menu.item(4))
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

	_upgrade_all_item.fade(!Packages::instance()->upgrades_available());
}
