/*********************************************************************
* This file is part of PackMan
*
* Copyright 2011-2020 AlanBuckley
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

#include "PathsWindow.h"
#include "Packages.h"
#include "MovePathWindow.h"
#include "PackManState.h"

#include "tbx/application.h"
#include "tbx/path.h"
#include "tbx/deleteonhidden.h"
#include "tbx/reporterror.h"
#include "tbx/stringutils.h"
#include "tbx/messagewindow.h"
#include "tbx/questionwindow.h"
#include "libpkg/pkgbase.h"
#include "libpkg/source_table.h"
#include <fstream>
#include <swis.h>
#include <cstdlib>

PathsWindow::PathsWindow() : _window("Paths"),
    _open(this, &PathsWindow::open),
    _move(this, &PathsWindow::move),
    _add(this, &PathsWindow::add),
    _remove(this, &PathsWindow::remove)
{
	_paths = _window.gadget(1);
	_paths.add_selection_listener(this);
	_window.add_about_to_be_shown_listener(this);
	_open_button = _window.gadget(2);
	_open_button.add_select_command(&_open);
	_move_button = _window.gadget(8);
	_move_button.add_select_command(&_move);
	tbx::ActionButton add_button = _window.gadget(9);
	add_button.add_select_command(&_add);
	_remove_button = _window.gadget(10);
	_remove_button.add_select_command(&_remove);

	tbx::OptionButton show_defn = _window.gadget(7);
	show_defn.add_state_listener(this);

	// Watch paths table for changes
	pkg::pkgbase *pkgbase = Packages::instance()->package_base();
	pkg::path_table &paths =pkgbase->paths();
	watch(paths);

    // Dispose of object when it is hidden
	_window.add_has_been_hidden_listener(new tbx::DeleteClassOnHidden<PathsWindow>(this));
	_window.add_has_been_hidden_listener(new tbx::DeleteObjectOnHidden());
}

PathsWindow::~PathsWindow()
{
	pkg::pkgbase *pkgbase = Packages::instance()->package_base();
	pkg::path_table &paths =pkgbase->paths();
	unwatch(paths);
}

/**
 * Load current list of sources as window is shown
 */
void PathsWindow::about_to_be_shown(tbx::AboutToBeShownEvent &event)
{
	show_paths(false);
}


/**
 * Paths table changed so updates
 */
void PathsWindow::handle_change(pkg::table& t)
{
	// paths table changed
	tbx::OptionButton show_defn = _window.gadget(7);
	show_paths(show_defn.on());
}


/**
 * Item selected on scroll list so update button or edit on double click
 */
void PathsWindow::scrolllist_selection(const tbx::ScrollListSelectionEvent &event)
{
	bool fade = (event.index() == -1);
	// Adjust click removes the selection on some versions of RISC OS.
	if (!fade && event.adjust() && _paths.first_selected() == -1) fade = true;
	_open_button.fade(fade);
	if (fade)
	{
	   _move_button.fade(fade);
	   _remove_button.fade();
	} else
	{
		std::string path = _paths.item_text(event.index());
		std::string::size_type eq_pos = path.find('=');
		std::string::size_type dot_pos = path.find('.');
		if (eq_pos != std::string::npos)
		{
		   path.erase(0, eq_pos+2);
		   if (dot_pos != std::string::npos && dot_pos > eq_pos) dot_pos = std::string::npos;
		}

		// Ensure we have the actual path
		if (!path.empty() && path[0] == '<') path = expand_path(path);
		// Simple test to check its not part of !Boot
		if (tbx::find_ignore_case(path, ".!BOOT.") == std::string::npos)
		{
			_move_button.fade(false);
		} else
		{
			_move_button.fade(true);
		}
		// Can only remove subpaths
		_remove_button.fade(dot_pos == std::string::npos);
	}
	if (!fade && event.double_click()) open();
}

/**
 * Get path for a specific index
 */
std::string PathsWindow::path(int index)
{
	return _paths.item_text(index);
}

/**
 * Add/Edit path
 *
 * @param index of source to replace or -1 to add
 * @param new source
 */
void PathsWindow::path(int index, const std::string &path)
{
	if (index == -1) _paths.add_item(path);
	else _paths.item_text(index, path);
}

/**
 * Open path
 */
void PathsWindow::open()
{
	std::string path;
	if (_paths.first_selected() != -1)
	{
		path = _paths.item_text(_paths.first_selected());
		std::string::size_type eq_pos = path.find('=');
		if (eq_pos != std::string::npos) path.erase(0, eq_pos+1);
	}

	if (path.find('@') != std::string::npos)
	{
		tbx::report_error("This path is expanded with the current selected"
				" package so it can not be opened here"
				);
	} else
	{
		std::string cmd("Filer_OpenDir ");
		cmd += path;
		try
		{
		    tbx::app()->os_cli(cmd);
		} catch(...)
		{
            // Shouldn't normally get here as filer reports the error
			tbx::report_error("Unable to open path " + path);
		}
	}
}

/**
 * Show move path window
 */
void PathsWindow::move()
{
	if (pmstate()->ok_to_move())
	{
	    _add_path = false;
		std::string path;
		if (_paths.first_selected() != -1)
		{
			path = _paths.item_text(_paths.first_selected());
			std::string::size_type eq_pos = path.find('=');
			if (eq_pos != std::string::npos)
			{
				_move_path = path.substr(0, eq_pos - 1);
				path.erase(0, eq_pos+2);

				tbx::SaveAs move_to("MoveTo");
				move_to.set_save_to_file_handler(this);
				move_to.add_has_been_hidden_listener(new tbx::DeleteObjectOnHidden());

				std::string title = move_to.title();
				std::string::size_type rep_pos = title.find('@');
				if (rep_pos != std::string::npos)
				{
					title.replace(rep_pos, 1, _move_path);
					move_to.title(title);
				}
				move_to.file_name(path);
				move_to.show_as_menu();
			}
		}
	}
}

/**
 * Show add path dialog
 */
void PathsWindow::add()
{
	if (pmstate()->ok_to_move())
	{
	    _add_path = true;
	    tbx::app()->catch_poll_exceptions(false);

		tbx::SaveAs add_path("AddPath");
		add_path.file_name("Location");
		add_path.set_save_to_file_handler(this);
		add_path.add_has_been_hidden_listener(new tbx::DeleteObjectOnHidden());
		add_path.show_as_menu();
	}
}

/**
 * Remove the selected path
 */
void PathsWindow::remove()
{
	if (pmstate()->ok_to_move())
	{
	    tbx::app()->catch_poll_exceptions(false);
		std::string path;
		if (_paths.first_selected() != -1)
		{
			path = _paths.item_text(_paths.first_selected());
			std::string::size_type eq_pos = path.find('=');
			if (eq_pos != std::string::npos && eq_pos > 0) path.erase(eq_pos-1);

			std::string msg(
			"When a logical path is removed all installed packages that\n"
			"use the path will be moved. If there is a large number of\n"
			"installed files this may take a while.\n\n"
			"Are you sure you wish to remove ");
			msg += path;
			msg += "?";
            tbx::show_question_as_menu(msg, "Remove Path", new RemovePath(path), 0, true);
        }
	}
}


/**
 * Handler for move path to a new location and add path
 */
void PathsWindow::saveas_save_to_file(tbx::SaveAs saveas, bool selection, std::string file_name)
{
   if (_add_path) do_add(saveas, file_name);
   else do_move(file_name);

   saveas.file_save_completed(true, file_name);
}

/**
 * Move path to another location
 */
void PathsWindow::do_move(const std::string &file_name)
{
    pkg::pkgbase *pkgbase = Packages::instance()->package_base();
	const pkg::path_table &paths =pkgbase->paths();

	tbx::Path old_path(paths(_move_path, ""));
	tbx::Path new_path(file_name);
	std::string old_leaf_name = old_path.leaf_name();

	if (old_path.canonical_equals(new_path))
	{
		tbx::show_message("You've have not moved to a new path!","","error");
	} else if (!old_leaf_name.empty() && old_leaf_name[0] == '!'
			&& old_leaf_name != new_path.leaf_name())
	{
		tbx::show_message("When moving an application the old and new leaf name must remain the same!","","error");
	} else
	{
		new MovePathWindow(_move_path, file_name);
	}
}

/**
 * A new path has been added
 */
void PathsWindow::do_add(tbx::SaveAs saveas, const std::string &file_name)
{
   	pkg::pkgbase *pkgbase = Packages::instance()->package_base();
	const pkg::path_table &paths =pkgbase->paths();
	tbx::WritableField path_field = saveas.window().gadget(2);
   std::string path_name = path_field.text();
   if (path_name.empty())
   {
       tbx::show_message("You must enter a logical path name","","error");
   } else if (*(file_name.rbegin()) == '.')
   {
	   tbx::show_message("You must enter a location for the path","", "error");
   } else
   {
      std::string::size_type dot_pos = path_name.find('.');
      if (dot_pos == std::string::npos)
      {
          tbx::show_message("The added logical path should a child of an existing logical path","", "error");
      } else if (*(path_name.rbegin()) == '.')
      {
         tbx::show_message("The logical path can not end with a '.'","","error");
      } else
      {
         std::string::size_type app_pos = path_name.find('!');
         std::string::size_type dot_after_app = (app_pos == std::string::npos) ? app_pos : path_name.find('.', app_pos);
         if (dot_after_app != std::string::npos)
         {
            tbx::show_message("The logical path cannot specify a location inside of an application","","error");
         } else if (const_cast<pkg::path_table *>(&paths)->find(path_name) != paths.end())
         {
            tbx::show_message("This logical path is already in the paths table\n"
                "Use move to move it instead.","","error");
         } else
         {
        	 bool is_child = true;
        	 try
        	 {
        		 std::string new_loc = paths(path_name, "");
        	 } catch(...)
        	 {
        		 is_child = false;
        	 }
        	 if (is_child)
        	 {
        		 new MovePathWindow(path_name, file_name);
        	 } else
        	 {
                 tbx::show_message("The added logical path should a child of an existing logical path","", "error");
        	 }
         }
      }
   }
}

/**
 * Actually remove a path
 */
void PathsWindow::RemovePath::execute()
{
   	pkg::pkgbase *pkgbase = Packages::instance()->package_base();
	pkg::path_table &paths =pkgbase->paths();
	std::string location;
	bool move_path = true;

	try
	{
		location = paths(_path, "");
		move_path = tbx::Path(location).exists();
	} catch(...)
	{
		// Can't resolve path so treat as it no longer exists
		move_path = false;
	}
	if (move_path)
	{
		new MovePathWindow(_path, "");
	} else
	{
		// Location on disc doesn't exist so just erase it
		paths.erase(_path);
		paths.commit();
	}
}

/**
 * Show definition
 */
void PathsWindow::option_button_state_changed(tbx::OptionButtonStateEvent &event)
{
	show_paths(event.turned_on());
	_open_button.fade(true);
	_move_button.fade(true);
}

/**
 * Show paths in dialogue
 *
 * @param defn true to show the path definition
 */
void PathsWindow::show_paths(bool defn)
{
	pkg::pkgbase *pkgbase = Packages::instance()->package_base();
	const pkg::path_table &paths =pkgbase->paths();
	_paths.clear();

	for (pkg::path_table::const_iterator i = paths.begin();
			i != paths.end(); ++i)
	{
		std::string path(i->first);
		path += " = ";
		if (defn)
		{
			path += i->second;
		} else
		{
			path += expand_path(i->second);
		}
		_paths.add_item(path);
	}
}

/**
 * Expand the give path to it's actual value
 *
 * @param path value to expand
 * @returns expanded value or original value if expansion failed
 */
std::string PathsWindow::expand_path(const std::string &path)
{
	if (path.empty() || path[0] != '<') return path;
	std::string::size_type gt_pos = path.find('>');
	if (gt_pos == std::string::npos) return path;

	std::string path_var = path.substr(1, gt_pos-1);
	char *path_val = getenv(path_var.c_str());
	if (path_val == 0) return path;

	std::string new_path = path_val + path.substr(gt_pos+1);

	std::string::size_type up_pos;
	while ((up_pos = new_path.find("^"))!= std::string::npos)
	{
		std::string::size_type erase_from = new_path.rfind(".", up_pos - 2);
		if (erase_from == std::string::npos) break; // Just to be safe
		new_path.erase(erase_from, up_pos - erase_from+1);
	}

	return new_path;
}

