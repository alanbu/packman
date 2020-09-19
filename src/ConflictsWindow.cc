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
 * ConflictsWindow.cc
 *
 *  Created on: 23-Jul-2009
 *      Author: alanb
 */

#include "ConflictsWindow.h"
#include "tbx/deleteonhidden.h"

ConflictsWindow::ConflictsWindow(const std::set<std::string> &pathnames) :
	_window("Conflicts"),
	_list(_window)
{
	_list.assign(pathnames.begin(), pathnames.end());

	// Delete window when it has been hidden
	_window.add_has_been_hidden_listener(new tbx::DeleteClassOnHidden<ConflictsWindow>(this));
}

ConflictsWindow::~ConflictsWindow()
{
	_window.delete_object();
}
