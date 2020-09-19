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
 * SearchWindow.cc
 *
 *  Created on: 20-Aug-2009
 *      Author: alanb
 */

#include "SearchWindow.h"
#include "MainWindow.h"
#include "tbx/actionbutton.h"
#include "tbx/deleteonhidden.h"

/**
 * Set up fields
 */
SearchWindow::SearchWindow(tbx::Object obj) : _window(obj)
{
	_text = _window.gadget(1);
	_current_section = _window.gadget(2);
	_current_status = _window.gadget(5);

	tbx::ActionButton search = _window.gadget(4);
	search.add_selected_listener(this);

	_window.add_about_to_be_shown_listener(this);
}

SearchWindow::~SearchWindow()
{
}

/**
 * Find currently selected package
 */
void SearchWindow::about_to_be_shown(tbx::AboutToBeShownEvent &event)
{
	_main = MainWindow::from_window(event.id_block().ancestor_object());
}

/**
 * Do search
 */
void SearchWindow::button_selected(tbx::ButtonSelectedEvent &event)
{
	_main->search(_text.text(), _current_status.on(), _current_section.on());

	if (!event.adjust())
	{
		_window.hide();
	}
}

