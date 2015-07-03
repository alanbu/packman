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

FindWindow::FindWindow() :
	_window("Find"),
	_find_command(this, &FindWindow::find),
	_filename_loader(_window.gadget(1))
{
	tbx::ActionButton find_button = _window.gadget(7);
	find_button.add_selected_command(&_find_command);
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
