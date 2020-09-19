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
#include "tbx/application.h"
#include "tbx/messagewindow.h"
#include "tbx/oserror.h"

#include "libpkg/binary_control.h"
#include "libpkg/pkgbase.h"
#include "libpkg/version.h"
#include "libpkg/component.h"

#include <fstream>
#include <cstdlib>

#include <swis.h>

/** The earliest standards version that uses the new components field. */
const pkg::version first_components_version("0.4");

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

    const pkg::binary_control *ctrl = 0;
    tbx::Object ancestor = event.id_block().ancestor_object();
    if (!ancestor.null())
    {
    	ISelectedPackage *sp = reinterpret_cast<ISelectedPackage *>(ancestor.client_handle());
    	if (sp != 0) ctrl = sp->selected_package();
    }

	if (ctrl != 0)
	{
		std::string pkgname = ctrl->pkgname();
		std::string title = "Components in package '" + pkgname.substr(0,100) + "'";
		_window.title(title);

		pkg::pkgbase *pb = Packages::instance()->package_base();
		std::string comp_str = ctrl->components();

		if (!comp_str.empty() || pkg::version(ctrl->standards_version()) >= first_components_version)
		{
			// Use component field
			if (!comp_str.empty())
			{
				try
				{
					std::vector<pkg::component> comps;
					pkg::parse_component_list(comp_str.begin(), comp_str.end(), &comps);
					for (std::vector<pkg::component>::iterator i = comps.begin(); i !=comps.end(); ++i)
					{
						if (i->flag(pkg::component::movable))
						{
							_apps.push_back(IconData(i->name()));
						}
					}
				} catch(...)
				{
					// Ignore exceptions and jist don't add components
				}

				_view.inserted(0, _apps.size());
			}
		} else
		{
			// Scan list of install files for applications
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

					//�Disregard the path component if it is the leafname
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
}

/**
 * Clear view when object has been hidden
 */
void AppsWindow::has_been_hidden(const tbx::EventInfo &event)
{
   _apps.clear();
    // Display is regenerated when reshown so don't update now
    _view.updates_enabled(false);
   _view.cleared();
   _view.updates_enabled(true);
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
	std::string::size_type f=logical_path.rfind(".");
	_name = logical_path.substr(f+1);

	tbx::Path ftpath(full_path());
	tbx::PathInfo ftinfo;
	ftpath.raw_path_info(ftinfo, true);
	int file_type;

	if (ftinfo.image_file())
	{
		// Image FS items also come back as directories so get raw file type
		file_type = ftinfo.raw_file_type();
	} else
	{
		file_type = ftinfo.file_type();
	}

	tbx::WimpSprite ws(file_type, _name);
	if (ws.exist()) _sprite_name = ws.name();
	else _sprite_name = "file_xxx"; // Use unknown file type
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
		try
		{
		   tbx::app()->os_cli(cmd);
		} catch (tbx::OsError &err)
		{
		   tbx::show_message("Unable to run !Boot for the application");
		}
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
		try
		{
		   tbx::app()->os_cli(cmd);
		} catch (tbx::OsError &err)
		{
		   tbx::show_message("Unable to run the application");
		}
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
		try
		{
		   tbx::app()->os_cli(cmd);
		} catch (tbx::OsError &err)
		{
		   tbx::show_message("Unable to run the !Help file for the application");
		}
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
		try
		{
		   tbx::app()->os_cli(cmd);
		} catch(tbx::OsError &err)
		{
		   tbx::show_message("Unable to open parent directory");
		}
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
	if (path.file_type() == tbx::FILE_TYPE_APPLICATION)
	{
		saver.save(where, path.leaf_name(), tbx::FILE_TYPE_APPLICATION, 1024);
	} else
	{
		tbx::show_message("Drag ignored! A Drag creates a stub, which is only available for an application");
	}
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
