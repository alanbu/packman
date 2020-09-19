/*********************************************************************
* This file is part of PackMan
*
* Copyright 2011-2020 AlanBuckley
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
/* PathsWindow.h
 *
 *  Created on: 23 May 2011
 *      Author: alanb
 */

#ifndef PATHSWINDOW_H_
#define PATHSWINDOW_H_

#include "tbx/abouttobeshownlistener.h"
#include "tbx/saveas.h"
#include "tbx/window.h"
#include "tbx/scrolllist.h"
#include "tbx/command.h"
#include "tbx/writablefield.h"
#include "tbx/actionbutton.h"
#include "tbx/optionbutton.h"
#include "tbx/optionbuttonstatelistener.h"
#include "libpkg/table.h"


class PathsWindow :
	tbx::AboutToBeShownListener,
	tbx::ScrollListSelectionListener,
	tbx::OptionButtonStateListener,
	tbx::SaveAsSaveToFileHandler,
	pkg::table::watcher
{
	tbx::Window _window;
	tbx::ScrollList _paths;
	tbx::ActionButton _open_button;
	tbx::ActionButton _move_button;
	tbx::ActionButton _remove_button;
	tbx::CommandMethod<PathsWindow> _open;
	tbx::CommandMethod<PathsWindow> _move;
	tbx::CommandMethod<PathsWindow> _add;
	tbx::CommandMethod<PathsWindow> _remove;

	std::string _move_path;
	bool _add_path;

    /**
     * Helper class to remove a path
     */
    class RemovePath : public tbx::Command
    {
       std::string _path;
       public:
          RemovePath(const std::string &path) : _path(path) {}
          virtual void execute();
    };

	// Event processing
	virtual void about_to_be_shown(tbx::AboutToBeShownEvent &event);
	virtual void scrolllist_selection(const tbx::ScrollListSelectionEvent &event);
	virtual void option_button_state_changed(tbx::OptionButtonStateEvent &event);
	virtual void saveas_save_to_file(tbx::SaveAs saveas, bool selection, std::string file_name);

	// Package table chaneg
	virtual void handle_change(pkg::table& t);

public:
	PathsWindow();
	virtual ~PathsWindow();

	std::string path(int index);
	void path(int index, const std::string &source);

	void show_paths(bool defn);

	// Implement commands on window
	void open();
	void move();
	void add();
	void remove();

private:
	// Helper
	std::string expand_path(const std::string &path);
	void do_move(const std::string &file_name);
	void do_add(tbx::SaveAs saveas, const std::string &file_name);
};

#endif /* PATHSWINDOW_H_ */
