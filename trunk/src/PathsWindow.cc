/*********************************************************************
* Copyright 2011 Alan Buckley
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
 * SourcesWindow.cc
 *
 *  Created on: 23 May 2011
 *      Author: alanb
 */

#include "PathsWindow.h"
#include "Packages.h"
#include "ErrorWindow.h"
#include "UpdateListWindow.h"
#include "tbx/path.h"
#include "tbx/deleteonhidden.h"
#include "tbx/reporterror.h"
#include "libpkg/pkgbase.h"
#include "libpkg/source_table.h"
#include <fstream>
#include <swis.h>

PathsWindow::PathsWindow() : _window("Paths"),
    _open(this, &PathsWindow::open)
{
	_paths = _window.gadget(1);
	_paths.add_selection_listener(this);
	_window.add_about_to_be_shown_listener(this);
	_open_button = _window.gadget(2);
	_open_button.add_select_command(&_open);

	tbx::OptionButton show_defn = _window.gadget(7);
	show_defn.add_state_listener(this);

    // Dispose of object when it is hidden
	_window.add_has_been_hidden_listener(new tbx::DeleteClassOnHidden<PathsWindow>(this));
	_window.add_has_been_hidden_listener(new tbx::DeleteObjectOnHidden());
}

PathsWindow::~PathsWindow()
{
}

/**
 * Load current list of sources as window is shown
 */
void PathsWindow::about_to_be_shown(tbx::AboutToBeShownEvent &event)
{
	show_paths(false);
}

/**
 * Item selected on scroll list so update button or edit on double click
 */
void PathsWindow::scrolllist_selection(const tbx::ScrollListSelectionEvent &event)
{
	bool fade = (event.index() == -1);
	_open_button.fade(fade);
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
		if (system(cmd.c_str()) != 0)
		{
			tbx::report_error("Unable to open path " + path);
		}
	}
}

/**
 * Show definition
 */
void PathsWindow::option_button_state_changed(tbx::OptionButtonStateEvent &event)
{
	show_paths(event.turned_on());
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

