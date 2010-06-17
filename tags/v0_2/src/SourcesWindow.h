/*********************************************************************
* Copyright 2010 Alan Buckley
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
/* SourcesWindow.h
 *
 *  Created on: 10 Feb 2010
 *      Author: alanb
 */

#ifndef SOURCESWINDOW_H_
#define SOURCESWINDOW_H_

#include "tbx/abouttobeshownlistener.h"
#include "tbx/window.h"
#include "tbx/scrolllist.h"
#include "tbx/command.h"
#include "tbx/writeablefield.h"
#include "tbx/actionbutton.h"
#include "tbx/optionbutton.h"

class SourceEdit;

class SourcesWindow :
	tbx::AboutToBeShownListener,
	tbx::ScrollListSelectionListener
{
	tbx::Window _window;
	tbx::ScrollList _sources;
	tbx::CommandMethod<SourcesWindow> _add, _edit, _remove, _save;
	SourceEdit *_editor;
	tbx::ActionButton _edit_button, _remove_button;
	tbx::OptionButton _update_list;

	// Event processing
	virtual void about_to_be_shown(tbx::AboutToBeShownEvent &event);
	virtual void scrolllist_selection(const tbx::ScrollListSelectionEvent &event);

public:
	SourcesWindow();
	virtual ~SourcesWindow();

	std::string source(int index);
	void source(int index, const std::string &source);

	// Implement commands on window
	void add();
	void edit();
	void remove();
	void save();
};

class SourceEdit
{
	SourcesWindow *_sources;
	int _edit_index;
	tbx::Window _window;
	tbx::WriteableField _url;
	tbx::CommandMethod<SourceEdit> _ok;

public:
	SourceEdit(SourcesWindow *sources);
	~SourceEdit();

	void add_show();
	void edit_show(int index);

	void ok();
};

#endif /* SOURCESWINDOW_H_ */
