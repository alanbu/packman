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

    bool is_small_size() const;
	void on_toggle_size();
};

#endif /* SUMMARYWINDOW_H_ */
