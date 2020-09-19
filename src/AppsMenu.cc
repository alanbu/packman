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
 * AppsMenu.cc
 *
 *  Created on: 30-Jul-2009
 *      Author: alanb
 */

#include "AppsMenu.h"
#include "AppsWindow.h"
#include "tbx/path.h"
#include "tbx/stringutils.h"

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

	/* Menu item number from Res file
	 * 1 - create stub
	 * 2 - create link
	 * 3 - copy
	 * 4 - move
	 * 10 - boot options
	 * 7 - boot
	 * 9 - run
	 * 5 - help
	 * 6 - view
	 */
	if (app_path.empty())
	{
		// Either no selection or multiple selection
		if (apps_window->multiple_apps_selected())
		{
			_app_item.text("Selection");
			_app_item.fade(false);
			tbx::Menu app_menu = _app_item.submenu();
			for (int i = 1; i <= 4; i++)
				app_menu.item(i).fade(true);
			app_menu.item(10).fade(true);
		} else
		{
			_app_item.text("Component ''");
			_app_item.fade(true);
		}
	} else
	{
		tbx::Path path(app_path);
		std::string app_text("Component '");
		app_text += path.leaf_name();
		app_text += "'";
		_app_item.text(app_text);
		_app_item.fade(false);
		tbx::Menu app_menu = _app_item.submenu();
		// Ensure options that are faded for multi-selections are enabled
		app_menu.item(2).fade(false);
		app_menu.item(3).fade(false);
        app_menu.item(10).fade(false);

        bool not_app = (path.file_type() != tbx::FILE_TYPE_APPLICATION);

        // Fade options only available to applications (not individual files)
        app_menu.item(1).fade(not_app);
        app_menu.item(7).fade(not_app);
        app_menu.item(5).fade(not_app);

		// Can't move anything from !Boot
		bool cant_move = app_path.empty() || (tbx::find_ignore_case(app_path, ".!Boot.") != std::string::npos);
		app_menu.item(4).fade(cant_move);
	}
}

/**
 * Remove selection if it was made by the menu button
 */
void AppsMenu::has_been_hidden(const tbx::EventInfo &hidden_event)
{
	AppsWindow *apps_window = AppsWindow::from_window(hidden_event.id_block().ancestor_object());
	apps_window->undo_menu_selection();
}
