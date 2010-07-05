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
 * AppsWindow.h
 *
 *  Created on: 29-Jul-2009
 *      Author: alanb
 */

#ifndef APPSWINDOW_H_
#define APPSWINDOW_H_

#include "tbx/abouttobeshownlistener.h"
#include "tbx/hasbeenhiddenlistener.h"
#include "tbx/view/tileview.h"
#include "tbx/view/iconitemrenderer.h"
#include "tbx/command.h"

/**
 * Class to show list of applications for a package
 * and allow them to be manipulated.
 */
class AppsWindow : tbx::AboutToBeShownListener, tbx::HasBeenHiddenListener
{
	tbx::Window _window;
	tbx::CommandMethod<AppsWindow> _boot_command;
	tbx::CommandMethod<AppsWindow> _run_command;
	tbx::CommandMethod<AppsWindow> _help_command;
	tbx::CommandMethod<AppsWindow> _view_command;

	// Internal class for data
	class IconData
	{
	public:
		IconData(const std::string full_path);

		std::string name() const {return _name;}
		std::string sprite_name() const {return _sprite_name;}
		std::string full_path() const {return _full_path;}

	private:
		std::string _name;
		std::string _sprite_name;
		std::string _full_path;
	};

	std::vector<IconData> _apps;

	// Classes for view
	tbx::view::MethodItemViewValue<std::string, std::vector<IconData>, IconData> _name_provider;
	tbx::view::MethodItemViewValue<std::string, std::vector<IconData>, IconData> _sprite_provider;
	tbx::view::IconItemRenderer _icon_renderer;
	tbx::view::TileView _view;

	// Selection
	tbx::view::MultiSelection _selection;

	void app_boot();
	void app_run();
	void app_help();
	void app_view();

	virtual void about_to_be_shown(tbx::AboutToBeShownEvent &event);
	virtual void has_been_hidden(const tbx::EventInfo &event);


public:
	AppsWindow(tbx::Object object);
	virtual ~AppsWindow();

	static AppsWindow *from_window(tbx::Window window);

	std::string selected_app_path() const;
	void undo_menu_selection();
	/**
	 * Return true if more than one item is selected
	 */
	bool multiple_apps_selected() const {return _selection.many();}
};

#endif /* APPSWINDOW_H_ */
