/*********************************************************************
* This file is part of PackMan
*
* Copyright 2014-2020 AlanBuckley
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
 * ChoicesWindow.cc
 *
 *  Created on: 6 Aug 2014
 *      Author: alanb
 */

#include "ChoicesWindow.h"
#include "Choices.h"
#include "Packages.h"
#include "ErrorWindow.h"

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
		_enable_logging(_window.gadget(4)),
		_use_proxy(_window.gadget(6)),
		_proxy_server(_window.gadget(8)),
		_do_not_proxy(_window.gadget(10))
{
	tbx::ActionButton ok_button = _window.gadget(3);
	ok_button.add_selected_listener(this);

	_use_proxy.add_state_listener(this);

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

	bool use_proxy = opts.use_proxy();
	_use_proxy.on(use_proxy);
	_proxy_server.text(opts.proxy_server());
	_do_not_proxy.text(opts.do_not_proxy());
	_proxy_server.fade(!use_proxy);
	_do_not_proxy.fade(!use_proxy);
	// Fade proxy labels as well
	_window.gadget(7).fade(!use_proxy);
	_window.gadget(9).fade(!use_proxy);
}

/**
 * Update choices selected
 */
void ChoicesWindow::button_selected(tbx::ButtonSelectedEvent &event)
{
	Choices &opts = choices();

	opts.update_prompt_days(update_list_choices[_update_list_prompt.selected_index()]);
	opts.enable_logging(_enable_logging.on());
	opts.use_proxy(_use_proxy.on());
	opts.proxy_server(_proxy_server.text());
	opts.do_not_proxy(_do_not_proxy.text());

	if (opts.modified())
	{
		if (!Choices::ensure_choices_dir())
		{
			new ErrorWindow("Unable to create", ChoicesDir, "Choices save failure");
		} else
		{
			opts.save();

			if (Packages::instance())
			{
				Packages::instance()->logging(opts.enable_logging());
			}
		}
	}
}

/**
 * Use proxy option button has been selected
 */
void ChoicesWindow::option_button_state_changed(tbx::OptionButtonStateEvent &event)
{
	bool use_proxy = event.turned_on();
	_proxy_server.fade(!use_proxy);
	_do_not_proxy.fade(!use_proxy);
	// Fade proxy labels as well
	_window.gadget(7).fade(!use_proxy);
	_window.gadget(9).fade(!use_proxy);
}
