/*********************************************************************
* Copyright 2015 Alan Buckley
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
 * InstallListLoadWindow.h
 *
 *  Created on: 16 Jul 2015
 *      Author: alanb
 */

#ifndef INSTALLLISTLOADWINDOW_H_
#define INSTALLLISTLOADWINDOW_H_

#include "InstallList.h"
#include "UpdateListWindow.h"
#include "tbx/command.h"
#include "tbx/hasbeenhiddenlistener.h"
#include "tbx/window.h"

class AgendaWindow;

/**
 * Window to load an install list, show summary and
 * allow it to be used
 */
class InstallListLoadWindow :
		tbx::HasBeenHiddenListener,
		UpdateListListener
{
	InstallList _list;
	tbx::Window _window;
	tbx::CommandMethod<InstallListLoadWindow> _show_agenda;
	tbx::CommandMethod<InstallListLoadWindow> _ok_command;
	AgendaWindow *_agenda;
	bool _adding_items;
	bool _add_paths;
	bool _add_sources;
	// Commands for update lists
	tbx::CommandMethod<InstallListLoadWindow> _update_lists_command;
	tbx::CommandMethod<InstallListLoadWindow> _add_packages_command;
public:
	InstallListLoadWindow(std::string filename);
	virtual ~InstallListLoadWindow();

	// used by sub window
	void reshow();

	bool check_drive(const std::string &drive);
	void remap_paths(std::vector<std::pair<std::string,std::string>> &mapping);

private:
	virtual void has_been_hidden(const tbx::EventInfo &event_info);
	virtual void lists_updated(bool success);

	void show_agenda();
	void ok();

	bool check_drives(std::set<std::string> &drives);
	void show_map_drives(std::set<std::string> &drives);
	void add_paths();
	void check_for_list_update();
	void update_lists();
	void add_packages();

};

#endif /* INSTALLLISTLOADWINDOW_H_ */
