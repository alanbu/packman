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
 * AppsMenu.cc
 *
 *  Created on: 30-Jul-2009
 *      Author: alanb
 */

#include "AppsMenu.h"
#include "AppsWindow.h"
#include "tbx/path.h"

AppsMenu::AppsMenu(tbx::Object object) :
	_apps_menu(object),
	_app_item(_apps_menu.item(0))
{
	_apps_menu.add_about_to_be_shown_listener(this);
	_apps_menu.add_has_been_hidden_listener(this);
}

AppsMenu::~AppsMenu()
{
}

/*
* Find currently selected application in Apps Window
* package and update menu and and app submenu.
*/
void AppsMenu::about_to_be_shown(tbx::AboutToBeShownEvent &event)
{
	AppsWindow *apps_window = AppsWindow::from_window(event.id_block().ancestor_object());
	std::string app_path = apps_window->selected_app_path();

	if (app_path.empty())
	{
		// Either no selection or multiple selection
		if (apps_window->multiple_apps_selected())
		{
			_app_item.text("Selection");
			_app_item.fade(false);
			tbx::Menu app_menu = _app_item.submenu();
			for (int i = 1; i <= 3; i++)
				app_menu.item(i).fade(true);
		} else
		{
			_app_item.text("Apps ''");
			_app_item.fade(true);
		}
	} else
	{
		tbx::Path path(app_path);
		std::string app_text("Apps '");
		app_text += path.leafname();
		app_text += "'";
		_app_item.text(app_text);
		_app_item.fade(false);
		tbx::Menu app_menu = _app_item.submenu();
		for (int i = 1; i <= 3; i++)
			app_menu.item(i).fade(false);
	}
}

/**
 * Remove selection if it was made by the menu button
 */
void AppsMenu::has_been_hidden(tbx::Object &object)
{
	AppsWindow *apps_window = AppsWindow::from_window(_apps_menu.ancestor_object());
	apps_window->undo_menu_selection();
}
