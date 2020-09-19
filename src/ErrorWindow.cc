/*********************************************************************
* This file is part of PackMan
*
* Copyright 2010-2020 AlanBuckley
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
 * ErrorWindow.cc
 *
 *  Created on: 11 Feb 2010
 *      Author: alanb
 */

#include "ErrorWindow.h"
#include "tbx/displayfield.h"
#include "tbx/deleteonhidden.h"

/**
 * Generic error window.
 *
 */
ErrorWindow::ErrorWindow(std::string errmsg, std::string title)
{
	init(errmsg, title);
}

ErrorWindow::ErrorWindow(const char *prefix, std::string errmsg, std::string title)
{
	std::string msg(prefix);
	msg += errmsg;
	init(msg, title);
}

void ErrorWindow::init(const std::string &errmsg, const std::string &title)
{
	_window = tbx::Window("Error");
	tbx::DisplayField msg(_window.gadget(1));
	_window.title(title);
	msg.text(errmsg);

	// Dump class and object when window is hidden
	_window.add_has_been_hidden_listener(new tbx::DeleteClassOnHidden<ErrorWindow>(this));
	_window.add_has_been_hidden_listener(new tbx::DeleteObjectOnHidden());
}

ErrorWindow::~ErrorWindow()
{
}
