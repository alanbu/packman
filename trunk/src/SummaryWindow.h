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
 * SummaryWindow.h
 *
 *  Created on: 30-Mar-2009
 *      Author: alanb
 */

#ifndef SUMMARYWINDOW_H_
#define SUMMARYWINDOW_H_

class MainWindow;
#include "SingleSelection.h"
#include "tbx/window.h"
#include "tbx/displayfield.h"
#include "tbx/textarea.h"

/**
 * Toolbar window to display a few more details
 */
class SummaryWindow : SingleSelectionListener
{
	MainWindow *_main;
	SingleSelection *_selection;
	tbx::DisplayField _name;
	tbx::DisplayField _installed;
	tbx::DisplayField _available;
	tbx::TextArea _description;

public:
	SummaryWindow(MainWindow *main, tbx::Window main_wnd, SingleSelection *selection);
	virtual ~SummaryWindow();

	virtual void selection_changed(unsigned int old_index, unsigned int new_index);

};

#endif /* SUMMARYWINDOW_H_ */
