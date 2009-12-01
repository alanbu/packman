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
#include "tbx/iconview.h"
#include "tbx/command.h"

/**
 * Class to show list of applications for a package
 * and allow them to be manipulated.
 */
class AppsWindow : tbx::AboutToBeShownListener
{
	tbx::Window _window;
	tbx::IconView _view;
	tbx::CommandMethod<AppsWindow> _boot_command;
	tbx::CommandMethod<AppsWindow> _run_command;
	tbx::CommandMethod<AppsWindow> _help_command;
	tbx::CommandMethod<AppsWindow> _view_command;

	class IconData : public tbx::IconViewItem
	{
	public:
		IconData(const std::string full_path);

		/**
		 * Return the text to display in the icon view
		 */
		virtual std::string display_text() const {return _name;}

		/**
		 * Return the name of the sprite to display in the icon view.
		 */
		virtual std::string display_sprite_name() const {return _sprite_name;}

		/**
		 * Return false to use the WIMP sprite area
		 */
		virtual bool use_client_sprite_area() const {return false;}

		/**
		 * Return path to item
		 */
		std::string full_path() const {return _full_path;}

	private:
		std::string _name;
		std::string _sprite_name;
		std::string _full_path;
	};

	void app_boot();
	void app_run();
	void app_help();
	void app_view();

public:
	AppsWindow(tbx::Object object);
	virtual ~AppsWindow();

	static AppsWindow *from_window(tbx::Window window);

	std::string selected_app_path() const;
	/**
	 * Return true if more than one item is selected
	 */
	bool multiple_apps_selected() const {return _view.multiple_selected();}

	void about_to_be_shown(tbx::AboutToBeShownEvent &event);
};

#endif /* APPSWINDOW_H_ */
