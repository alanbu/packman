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
 * MapDrivesWindow.h
 *
 *  Created on: 13 Nov 2015
 *      Author: alanb
 */

#ifndef MAPDRIVESWINDOW_H_
#define MAPDRIVESWINDOW_H_

#include "tbx/window.h"
#include "tbx/command.h"

#include <vector>
#include <set>
#include <algorithm>

class InstallListLoadWindow;

/**
 * Class to show a list of the drives that need to be mapped to
 * the new machine.
 */
class MapDrivesWindow
{
	InstallListLoadWindow *_ill_window;
	std::vector<std::pair<std::string,std::string>> _drive_map;
	tbx::Window _window;
	tbx::CommandMethod<MapDrivesWindow> _cancel_command;
	tbx::CommandMethod<MapDrivesWindow> _boot_command;
	tbx::CommandMethod<MapDrivesWindow> _map_command;
	int _num_bad;
public:
	MapDrivesWindow(InstallListLoadWindow *ill_window, std::set<std::string> &bad_drives);
	virtual ~MapDrivesWindow();

	void cancel();
	void make_boot();
	void map();
};

#endif /* MAPDRIVESWINDOW_H_ */
