/*********************************************************************
* Copyright 2009 Alan Buckley
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
	_current = _window.gadget(2);

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
	_main->search(_text.text(), _current.on());

	if (!event.adjust())
	{
		_window.hide();
	}
}

