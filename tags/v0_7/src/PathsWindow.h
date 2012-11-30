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
/* PathsWindow.h
 *
 *  Created on: 23 May 2011
 *      Author: alanb
 */

#ifndef PATHSWINDOW_H_
#define PATHSWINDOW_H_

#include "tbx/abouttobeshownlistener.h"
#include "tbx/saveas.h"
#include "tbx/window.h"
#include "tbx/scrolllist.h"
#include "tbx/command.h"
#include "tbx/writablefield.h"
#include "tbx/actionbutton.h"
#include "tbx/optionbutton.h"
#include "tbx/optionbuttonstatelistener.h"
#include "libpkg/table.h"


class PathsWindow :
	tbx::AboutToBeShownListener,
	tbx::ScrollListSelectionListener,
	tbx::OptionButtonStateListener,
	tbx::SaveAsSaveToFileHandler,
	pkg::table::watcher
{
	tbx::Window _window;
	tbx::ScrollList _paths;
	tbx::ActionButton _open_button;
	tbx::ActionButton _move_button;
	tbx::CommandMethod<PathsWindow> _open;
	tbx::CommandMethod<PathsWindow> _move;

	std::string _move_path;

	// Event processing
	virtual void about_to_be_shown(tbx::AboutToBeShownEvent &event);
	virtual void scrolllist_selection(const tbx::ScrollListSelectionEvent &event);
	virtual void option_button_state_changed(tbx::OptionButtonStateEvent &event);
	virtual void saveas_save_to_file(tbx::SaveAs saveas, bool selection, std::string file_name);

	// Package table chaneg
	virtual void handle_change(pkg::table& t);

public:
	PathsWindow();
	virtual ~PathsWindow();

	std::string path(int index);
	void path(int index, const std::string &source);

	void show_paths(bool defn);

	// Implement commands on window
	void open();
	void move();

private:
	// Helper
	std::string expand_path(const std::string &path);
};

#endif /* PATHSWINDOW_H_ */
