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
#include "tbx/view/selection.h"
#include "tbx/window.h"
#include "tbx/displayfield.h"
#include "tbx/textarea.h"
#include "tbx/toolaction.h"
#include "tbx/command.h"
/**
 * Toolbar window to display a few more details
 */
class SummaryWindow : tbx::view::SelectionListener
{
	MainWindow *_main;
	tbx::view::Selection *_selection;
	tbx::DisplayField _name;
	tbx::DisplayField _installed;
	tbx::DisplayField _available;
	tbx::TextArea _description;
	tbx::ToolAction _toggle_size;
	tbx::CommandMethod<SummaryWindow> _toggle_command;
	int _height;

public:
	SummaryWindow(MainWindow *main, tbx::Window main_wnd, tbx::view::Selection *selection);
	virtual ~SummaryWindow();

	virtual void selection_changed(const tbx::view::SelectionChangedEvent &event);
	void set_noselection_text();
    void set_selection_text(bool description);

private:
	void on_toggle_size();
};

#endif /* SUMMARYWINDOW_H_ */
