/*********************************************************************
* This file is part of PackMan
*
* Copyright 2010-2020 AlanBuckley
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
/* SourcesWindow.h
 *
 *  Created on: 10 Feb 2010
 *      Author: alanb
 */

#ifndef SOURCESWINDOW_H_
#define SOURCESWINDOW_H_

#include "SourcesFile.h"
#include "tbx/abouttobeshownlistener.h"
#include "tbx/window.h"
#include "tbx/scrolllist.h"
#include "tbx/command.h"
#include "tbx/writablefield.h"
#include "tbx/actionbutton.h"
#include "tbx/optionbutton.h"


class SourceEdit;

class SourcesWindow :
	tbx::AboutToBeShownListener,
	tbx::ScrollListSelectionListener
{
	tbx::Window _window;
	tbx::ScrollList _sources;
	tbx::CommandMethod<SourcesWindow> _add, _edit, _remove, _save, _enable;
	SourceEdit *_editor;
	tbx::ActionButton _edit_button, _remove_button, _enable_button;
	tbx::OptionButton _update_list;
	SourcesFile _sources_file;
	bool _ignore_next_select;

    class ShowKnownSources : public tbx::Command
    {
       public:
          virtual void execute();
    } _show_known_sources;


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
	void enable();
	void save();
};

class SourceEdit
{
	SourcesWindow *_sources;
	int _edit_index;
	tbx::Window _window;
	tbx::WritableField _url;
	tbx::CommandMethod<SourceEdit> _ok;

public:
	SourceEdit(SourcesWindow *sources);
	~SourceEdit();

	void add_show();
	void edit_show(int index);

	void ok();
};

#endif /* SOURCESWINDOW_H_ */
