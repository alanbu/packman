/*********************************************************************
* Copyright 2009-2011 Alan Buckley
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
 * AppsWindow.cc
 *
 *  Created on: 29-Jul-2009
 *      Author: alanb
 */

#include "AppsWindow.h"
#include "MainWindow.h"
#include "Packages.h"
#include "CreateStub.h"

#include "tbx/deleteonhidden.h"
#include "tbx/sprite.h"
#include "tbx/path.h"

#include "libpkg/binary_control.h"
#include "libpkg/pkgbase.h"

#include <fstream>
#include <cstdlib>

AppsWindow::AppsWindow(tbx::Object obj) :
	_window(obj),
	// Commands
	_boot_command(this, &AppsWindow::app_boot),
	_run_command(this, &AppsWindow::app_run),
	_help_command(this, &AppsWindow::app_help),
	_view_command(this, &AppsWindow::app_view),
	// View setup
	_name_provider(&_apps, &IconData::name),
	_sprite_provider(&_apps, &IconData::sprite_name),
	_icon_renderer(&_name_provider, &_sprite_provider, false),
	_view(_window, &_icon_renderer)
{
	_window.add_about_to_be_shown_listener(this);
	_window.add_has_been_hidden_listener(this);

	_window.add_command(10, &_boot_command);
	_window.add_command(11, &_run_command);
	_window.add_command(12, &_help_command);
	_window.add_command(13, &_view_command);

	_window.client_handle(this);

	_view.selection(&_selection);
	_view.menu_selects(true);
	_view.add_click_listener(this);
}

AppsWindow::~AppsWindow()
{
}

/**
 * Return the apps window from the toolbox window
 */
AppsWindow *AppsWindow::from_window(tbx::Window window)
{
	return static_cast<AppsWindow *>(window.client_handle());
}

/**
 * Find currently selected package and update menu
 */
void AppsWindow::about_to_be_shown(tbx::AboutToBeShownEvent &event)
{
   _apps.clear();
   _view.cleared();

	MainWindow *main = MainWindow::from_window(event.id_block().ancestor_object());
	const pkg::binary_control *ctrl = main->selected_package();


	if (ctrl != 0)
	{
		std::string pkgname = ctrl->pkgname();
		std::string title = "Applications in package '" + pkgname.substr(0,100) + "'";
		_window.title(title);

		// Open manifest.
		pkg::pkgbase *pb = Packages::instance()->package_base();
		std::string files_pathname=pb->info_pathname(pkgname)+std::string(".Files");
		std::ifstream in(files_pathname.c_str());
		std::string last_pathname;

		while (in)
		{
			// Read pathname from manifest.
			std::string line;
			getline(in,line);

			// Look for exclamation mark at start of path component.
			std::string::size_type f=line.find(".!");
			if (f!=std::string::npos)
			{
				// Isolate pathname up to and including the matching
				// path component.
				std::string::size_type e=line.find(".",f+1);
				std::string src_pathname=line.substr(0,e);

				// Disregard the path component if it is the leafname
				// (that is, if it refers to a file rather than a directory).
				if (e!=std::string::npos)
				{
					if (last_pathname != src_pathname)
					{
						last_pathname = src_pathname;
						// Add to window if pathname not already known.
						_apps.push_back(IconData(src_pathname));
					}
				}
			}
		}
		_view.inserted(0, _apps.size());
	}
}

/**
 * Clear view when object has been hidden
 */
void AppsWindow::has_been_hidden(const tbx::EventInfo &event)
{
   _apps.clear();
   _view.cleared();
}

/**
 * And item in the item view has been clicked
 */
void AppsWindow::itemview_clicked(const tbx::view::ItemViewClickEvent &event)
{
	if (event.click_event().is_select_double()) app_run();
	else if (event.click_event().is_adjust_double())
	{
		app_run();
		_window.hide();
	} else if (event.click_event().is_select_drag())
	{
		if (event.item_hit())
		{
			// Drag sprite
			tbx::WimpSprite app_sprite(_apps[event.index()].sprite_name());
			tbx::Size sprite_size = app_sprite.size();
			tbx::BBox start_box(event.click_event().point(), sprite_size);
			start_box.move(-app_sprite.width()/2, -app_sprite.height()/2);
			_window.drag_sprite(app_sprite, start_box, this, tbx::Window::DSFLAG_DROP_SHADOW);
		}
	}
}


/**
 * Create icon for window
 */
AppsWindow::IconData::IconData(std::string logical_path) :
	_logical_path(logical_path)
{
	std::string::size_type f=logical_path.find(".!");
	_name = logical_path.substr(f+1);
	_sprite_name = _name;

    tbx::WimpSprite ws(_sprite_name);
	if (!ws.exist()) _sprite_name = "application";
}

/**
 * Retrieve full path on demand as it may be changed
 * by the user moving the application while the apps window is shown.
 */
std::string AppsWindow::IconData::full_path() const
{
	pkg::pkgbase *pb = Packages::instance()->package_base();
	return pb->paths()(_logical_path,"");
}


/**
 * Return pathname of first selected application
 *
 * returns empty string if nothing is selected
 */
std::string AppsWindow::selected_app_path() const
{
	std::string pathname;
	if (_selection.one())
		pathname = _apps[_selection.first()].full_path();

	return pathname;
}

/**
 * Return logical pathname of the first selected application
 *
 * The logical path is the path used to lookup the actual
 * location on disc from the paths file.
 *
 * returns empty string if nothing is selected
 */
std::string AppsWindow::selected_app_logical_path() const
{
	std::string pathname;
	if (_selection.one())
		pathname = _apps[_selection.first()].logical_path();

	return pathname;
}


/**
 * Undo the last selection if it was caused
 * by the menu button
 */
void AppsWindow::undo_menu_selection()
{
	_view.clear_menu_selection();
}


/**
 * Run boot file for the selected applications
 */
void AppsWindow::app_boot()
{
	for (tbx::view::Selection::Iterator i = _selection.begin();
	     i != _selection.end(); ++i)
	{
		std::string cmd("Filer_Run ");
		cmd += _apps[*i].full_path();
		cmd += ".!Boot";
		int result = system(cmd.c_str());
		(void)result; // No useful return code
	}
}

/**
 * Run the selected application
 */
void AppsWindow::app_run()
{
	for (tbx::view::Selection::Iterator i = _selection.begin();
	     i != _selection.end(); ++i)
	{
		std::string cmd("Filer_Run ");
		cmd += _apps[*i].full_path();
		cmd += ".!Run";
		int result = system(cmd.c_str());
		(void)result; // No useful return code
	}
}
/**
 * Run Help file for the selected application
 */
void AppsWindow::app_help()
{
	for (tbx::view::Selection::Iterator i = _selection.begin();
	     i != _selection.end(); ++i)
	{
		std::string cmd("Filer_Run ");
		cmd += _apps[*i].full_path();
		cmd += ".!Help";
		int result = system(cmd.c_str());
		(void)result; // No useful return code
	}
}
/**
 * View location of the selected application
 */
void AppsWindow::app_view()
{
	for (tbx::view::Selection::Iterator i = _selection.begin();
	     i != _selection.end(); ++i)
	{
		tbx::Path path(_apps[*i].full_path());
		std::string cmd("Filer_OpenDir ");
		cmd += path.parent().name();
		int result = system(cmd.c_str());
		(void)result; // No useful return code
	}
}


/**
 * Drag has finished so create a stub application
 */
void AppsWindow::drag_finished (const tbx::BBox &final)
{
	tbx::Saver saver;
	tbx::PointerInfo where(true,false);
	saver.set_save_to_file_handler(this);

	tbx::view::Selection::Iterator i = _selection.begin();
	tbx::Path path(_apps[*i].full_path());
	saver.save(where, path.leaf_name(), tbx::FILE_TYPE_APPLICATION, 1024);
}


/**
 * Save file - or in this case create the stub
 */
void AppsWindow::saver_save_to_file(tbx::Saver saver, std::string file_name)
{
	tbx::view::Selection::Iterator i = _selection.begin();
	tbx::Path source_path(_apps[*i].full_path());
	create_application_stub(source_path, file_name);

	saver.file_save_completed(true, file_name);
}
