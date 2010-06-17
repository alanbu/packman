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
 * UpdateListWindow.h
 *
 *  Created on: 23-Jul-2009
 *      Author: alanb
 */

#ifndef UPDATELISTWINDOW_H_
#define UPDATELISTWINDOW_H_

#include "tbx/window.h"
#include "tbx/displayfield.h"
#include "tbx/slider.h"
#include "tbx/actionbutton.h"
#include "tbx/command.h"

#include "libpkg/thread.h"
#include "libpkg/update.h"

/**
 * Class to show window and update list of available packages
 */
class UpdateListWindow : pkg::thread
{
	tbx::Window _window;
	tbx::DisplayField _action;
	tbx::Slider _progress;
	tbx::ActionButton _cancel_button;
    static UpdateListWindow *_instance;

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

public:
    UpdateListWindow();
	virtual ~UpdateListWindow();

    static bool showing() {return (_instance != 0);}

    void cancel();

    virtual void poll();
};

#endif /* UPDATELISTWINDOW_H_ */
