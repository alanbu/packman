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
 * UpdateListWindow.h
 *
 *  Created on: 23-Jul-2009
 *      Author: alanb
 */

#ifndef UPDATELISTWINDOW_H_
#define UPDATELISTWINDOW_H_

#include "Commands.h"

#include "tbx/window.h"
#include "tbx/displayfield.h"
#include "tbx/slider.h"
#include "tbx/actionbutton.h"
#include "tbx/command.h"

#include "libpkg/thread.h"
#include "libpkg/update.h"

#include <set>
#include <tr1/memory>

class LogViewer;

/**
 * Listener for completion of update lists
 */
class UpdateListListener
{
public:
	virtual ~UpdateListListener() {}
	/**
	 * Called when list update has finished
	 *
	 * @param success true if list update succeeded
	 */
	virtual void lists_updated(bool success) = 0;
};

/**
 * Class to show window and update list of available packages
 */
class UpdateListWindow : pkg::thread
{
    static UpdateListWindow *_instance;

	tbx::Window _window;
	tbx::DisplayField _action;
	tbx::Slider _progress;
	tbx::ActionButton _cancel_button;
	tbx::ActionButton _whats_new;
	tbx::ActionButton _upgrade_all;
	tbx::ActionButton _show_log;
    std::set<std::string> _whats_old;
    ShowWhatsNewCommand _show_whats_new;

    class CancelCommand : public tbx::Command
    {
    	UpdateListWindow *_me;
    public:
    	CancelCommand(UpdateListWindow *me) : _me(me) {};
    	virtual void execute();
    } _cancel_command;

    /**
     * Class to poll libpkg during null events
     */
    class PkgThreadRunner : public tbx::Command
    {
    public:
    	virtual void execute();
    } _thread_runner;

    /** The current update operation, or 0 if none. */
	pkg::update* _upd;

	/** The last known state of the update operation.
	 * This field is used to detect changes to the update operation state. */
	pkg::update::state_type _state;

	/** The log file for this operation */
	std::tr1::shared_ptr<pkg::log> _log;
	/** The log viewer shown from this window */
	LogViewer *_log_viewer;

	/** Binding from log button to show log command */
	tbx::CommandMethod<UpdateListWindow> _show_log_command;

	/** Listener for update finished */
	UpdateListListener *_listener;

public:
    UpdateListWindow();
	virtual ~UpdateListWindow();

	static UpdateListWindow *instance() {return _instance;}

    static bool showing() {return (_instance != 0);}
    static bool running();

    void cancel();

    virtual void poll();

    void listener(UpdateListListener *listener);

private:
    bool write_whats_new();
    void show_log();
};

#endif /* UPDATELISTWINDOW_H_ */
