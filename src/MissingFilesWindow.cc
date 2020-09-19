/*********************************************************************
* This file is part of PackMan
*
* Copyright 2013-2020 AlanBuckley
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

#include "MissingFilesWindow.h"
#include "tbx/deleteonhidden.h"

MissingFilesWindow::MissingFilesWindow(const std::vector<std::string> &pathnames) :
	_window("Missing"),
	_list(_window)
{
	_list.assign(pathnames.begin(), pathnames.end());

	// Delete window when it has been hidden
	_window.add_has_been_hidden_listener(new tbx::DeleteClassOnHidden<MissingFilesWindow>(this));
}

MissingFilesWindow::~MissingFilesWindow()
{
	_window.delete_object();
}
