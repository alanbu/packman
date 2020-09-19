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
 * FindWindow.cc
 *
 *  Created on: 13 May 2015
 *      Author: alanb
 */

#include "FindWindow.h"
#include "Finder.h"
#include "FileFoundWindow.h"
#include "FindResultsWindow.h"
#include "tbx/actionbutton.h"
#include "tbx/writablefield.h"
#include "tbx/radiobutton.h"
#include "tbx/optionbutton.h"
#include "tbx/messagewindow.h"
#include "tbx/path.h"
#include "tbx/hourglass.h"
#include "tbx/loader.h"
#include "tbx/deleteonhidden.h"

FindWindow::FindWindow() :
	_window("Find"),
	_find_command(this, &FindWindow::find),
	_filename_loader(_window.gadget(1))
{
	tbx::ActionButton find_button = _window.gadget(7);
	find_button.add_selected_command(&_find_command);

	// Dump class and object when window is hidden
	_window.add_has_been_hidden_listener(new tbx::DeleteClassAndObjectOnHidden<FindWindow>(this));
}

FindWindow::FindWindow(const std::string &find_text) : FindWindow()
{
	tbx::WritableField(_window.gadget(1)).text(find_text);
}

FindWindow::~FindWindow()
{
}

/**
 * Run the find
 */
void FindWindow::find()
{
	std::string find_text = tbx::WritableField(_window.gadget(1)).text();
	if (find_text.empty())
	{
		tbx::show_message("Nothing entered to find","PackMan - Find", "error");
		return;
	}

	tbx::Path find_path(find_text);

	bool exact = tbx::OptionButton(_window.gadget(6)).on();
	bool search_comp = tbx::RadioButton(_window.gadget(2)).on();
	bool search_inst = tbx::RadioButton(_window.gadget(3)).on();
	if (!search_comp && !search_inst)
	{
		// Search both if neither of the first two buttons is on
		search_comp = true;
		search_inst = true;
	}

	tbx::Hourglass hg(true);
	Finder finder;

	if (search_comp && finder.find_component(find_text, exact))
	{
		if (finder.results().size() > 1)
		{
			new FindResultsWindow(find_text, finder.results());
		} else
		{
			new FileFoundWindow(find_text, finder.results()[0].package, finder.results()[0].logical_path, "Component Found");
		}
	} else if (search_inst && finder.find_installed_file(find_text, exact))
	{
		if (finder.results().size() > 1)
		{
			new FindResultsWindow(find_text, finder.results());
		} else
		{
			new FileFoundWindow(find_text, finder.results()[0].package, finder.results()[0].logical_path, "Installed File Found");
		}
	} else
	{
		std::string msg("No ");
		if (search_comp)
		{
			msg += "component";
			if (search_inst) msg += " or ";
		}
		if (search_inst) msg += "installed file";
		msg += " matching " + find_text + " found.";
		hg.off();
		tbx::show_message(msg, "PackMan - Find", "information");
	}
}
