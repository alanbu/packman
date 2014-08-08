/*********************************************************************
* Copyright 2014 Alan Buckley
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
 * ChoicesWindow.cc
 *
 *  Created on: 6 Aug 2014
 *      Author: alanb
 */

#include "ChoicesWindow.h"
#include "Choices.h"
#include "Packages.h"

#include "tbx/window.h"
#include "tbx/actionbutton.h"

// List of days in update list prompt string set
const int NUM_UPDATE_LIST_CHOICES = 6;
static int update_list_choices[NUM_UPDATE_LIST_CHOICES] =
		{-1, 0, 7, 14, 30, 90};

ChoicesWindow *ChoicesWindow::_instance = 0;

/**
 * Construct choices window
 */
ChoicesWindow::ChoicesWindow() :
		_window("Choices"),
		_update_list_prompt(_window.gadget(1)),
		_enable_logging(_window.gadget(4))
{
	tbx::ActionButton ok_button = _window.gadget(3);
	ok_button.add_selected_listener(this);

	_window.add_about_to_be_shown_listener(this);
	_instance = this;
}

ChoicesWindow::~ChoicesWindow()
{
	_instance = 0;
}

void ChoicesWindow::show()
{
	if (_instance) _instance->_window.show();
	else new ChoicesWindow();
}

/**
 * Window about to be shown so update fields for
 * current choices value
 */
void ChoicesWindow::about_to_be_shown(tbx::AboutToBeShownEvent &event)
{
	Choices &opts = choices();

	int index;
	int update_prompt_days = opts.update_prompt_days();
	for (index = 0; index < NUM_UPDATE_LIST_CHOICES; index++)
	{
		if (update_prompt_days <= update_list_choices[index]) break;
	}
	// In case a larger value get's into choice by accident of from a
	// different PackMan release
	if (index == NUM_UPDATE_LIST_CHOICES) index--;

	_update_list_prompt.selected_index(index);

	_enable_logging.on(opts.enable_logging());
}

/**
 * Update choices selected
 */
void ChoicesWindow::button_selected(tbx::ButtonSelectedEvent &event)
{
	Choices &opts = choices();

	opts.update_prompt_days(update_list_choices[_update_list_prompt.selected_index()]);
	opts.enable_logging(_enable_logging.on());

	if (opts.modified())
	{
		opts.save();

		if (Packages::instance())
		{
			Packages::instance()->logging(opts.enable_logging());
		}
	}
}

