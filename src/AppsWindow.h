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
#include "tbx/draghandler.h"
#include "tbx/saver.h"

/**
 * Class to show list of applications for a package
 * and allow them to be manipulated.
 */
class AppsWindow :
	tbx::AboutToBeShownListener,
	tbx::HasBeenHiddenListener,
	tbx::view::ItemViewClickListener,
	tbx::DragHandler,
	tbx::SaverSaveToFileHandler
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
		IconData(const std::string logical_path);

		std::string name() const {return _name;}
		std::string sprite_name() const {return _sprite_name;}
		std::string full_path() const;
		std::string logical_path() const {return _logical_path;}

	private:
		std::string _name;
		std::string _sprite_name;
		std::string _logical_path;
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
	virtual void itemview_clicked(const tbx::view::ItemViewClickEvent &event);
	virtual void drag_finished (const tbx::BBox &final);
	virtual void saver_save_to_file(tbx::Saver saver, std::string file_name);


public:
	AppsWindow(tbx::Object object);
	virtual ~AppsWindow();

	static AppsWindow *from_window(tbx::Window window);

	std::string selected_app_path() const;
	std::string selected_app_logical_path() const;

	void undo_menu_selection();
	/**
	 * Return true if more than one item is selected
	 */
	bool multiple_apps_selected() const {return _selection.many();}
};

#endif /* APPSWINDOW_H_ */
