/*********************************************************************
* This file is part of PkgTest (PackMan unit testing)
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
 * CommitWindow.h
 *
 *  Created on: 27-Mar-2009
 *      Author: alanb
 */

#ifndef COMMITWINDOW_H_
#define COMMITWINDOW_H_

#include "tbx/window.h"
#include "tbx/displayfield.h"
#include "tbx/slider.h"
#include "tbx/actionbutton.h"
#include "tbx/command.h"

// libpkg commit.h doesn't currently define set and map and puts string
// in no namespace
#include <map>
#include <set>
#include <tr1/memory>

using namespace std;
#include "libpkg/commit.h"
#include "libpkg/thread.h"
#include "libpkg/log.h"

#define PKG_TEST

#ifndef PKG_TEST
class LogViewer;
#endif

/**
 * Class to show a progress window and commit changes
 * from install
 */
class CommitWindow : public pkg::thread
{
	tbx::Window _window;
	tbx::DisplayField _action;
	tbx::Slider _progress;
	tbx::ActionButton _cancel_button;
	tbx::ActionButton _show_log;

    static CommitWindow *_instance;

    class CancelCommand : public tbx::Command
    {
    	CommitWindow *_me;
    public:
    	CancelCommand(CommitWindow *me) : _me(me) {};
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

    pkg::commit *_commit;
    pkg::commit::state_type _state;

	/** The log file for this operation */
	std::tr1::shared_ptr<pkg::log> _log;
	/** The log viewer shown from this window */
#ifndef PKG_TEST
	LogViewer *_log_viewer;
#endif

	/** Binding from log button to show log command */
	tbx::CommandMethod<CommitWindow> _show_log_command;

private:
    void open_conflicts_window();

public:
    CommitWindow();
	virtual ~CommitWindow();

    static bool showing() {return (_instance != 0);}
    static CommitWindow *instance() {return _instance;}
    bool done() const {return _commit == 0;}
    static bool running();

    void close();

    virtual void poll();

    void show_log();
};

#endif /* COMMITWINDOW_H_ */
