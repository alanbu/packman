/*********************************************************************
* This file is part of PackMan
*
* Copyright 2015-2020 AlanBuckley
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
