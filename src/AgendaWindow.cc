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
/*/*
 * AgendaWindow.cc
 *
 *  Created on: 23-Jul-2009
 *      Author: alanb
 */

#include "AgendaWindow.h"

AgendaWindow::AgendaWindow() :
    _window("Agenda"),
	_list(_window)
{
}

/**
 * Populate the agenda and show the window.
 *
 * @param list the install list with what to do on.
 */
void AgendaWindow::show(const InstallList &list)
{
	_list.clear();
	std::string msg;

	if (list.paths_count())
	{
		for (auto path : list.paths())
		{
			msg = "Add Path '" + path.first + "', location '" + path.second + "'";
			_list.push_back(msg);
		}
	}

	if (!list.clash_paths().empty())
	{
		for (auto path : list.clash_paths())
		{
			msg = "Ignoring Path '" + path.first + "' as it clashes with the existing path";
			_list.push_back(msg);
		}
	}

	if (list.sources_count())
	{
		for (auto source : list.sources())
		{
			msg = "Add Source '" + source + "'";
			_list.push_back(msg);
		}
	}

	if (!list.unavailable_packages().empty())
	{
		_list.push_back("Prompt to update package list");
	}

	if (list.packages_count())
	{
		for (auto pack : list.packages())
		{
			msg = "Add Package '" +  pack.name + "'";
			_list.push_back(msg);
		}
	}

	if (!list.unavailable_packages().empty())
	{
		for (auto pack : list.unavailable_packages())
		{
			msg = "May Add Package '"+  pack.name + "' if found after update lists";
			_list.push_back(msg);
		}
	}

	_window.show();
}

AgendaWindow::~AgendaWindow()
{
	_window.delete_object();
}
