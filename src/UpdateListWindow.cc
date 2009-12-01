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
 * UpdateListWindow.cc
 *
 *  Created on: 23-Jul-2009
 *      Author: alanb
 */

#include "UpdateListWindow.h"
#include "Packages.h"
#include "tbx/application.h"
#include "libpkg/download.h"

UpdateListWindow *UpdateListWindow::_instance = 0;

UpdateListWindow::UpdateListWindow() :
    _window("UpdateList"),
    _action(_window.gadget(1)),
    _progress(_window.gadget(2)),
    _cancel_button(_window.gadget(3)),
    _cancel_command(this),
    _upd(0),
    _state(pkg::update::state_done)
{
   _instance = this;
   _cancel_button.add_select_command(&_cancel_command);

   pkg::pkgbase *package_base = Packages::instance()->package_base();

	_upd=new pkg::update(*package_base);

	// Start libpkg threads
	tbx::app()->add_idle_command(&_thread_runner);

    _window.show();
}

UpdateListWindow::~UpdateListWindow()
{
	delete _upd;
}

/**
 * Update UI with current state of commit
 */
void UpdateListWindow::poll()
{
	if (_upd)
	{
		// Update progress bar.
		// (Must do this before checking state, in case the update
		// operation is deleted.)
		pkg::update::size_type numerator=_upd->bytes_done();
		pkg::update::size_type denominator=_upd->bytes_total();
		while (denominator>0xfff)
		{
			numerator>>=1;
			denominator>>=1;
		}
		_progress.value(numerator * 100 / ((denominator)?denominator:1));

		// Check whether update operation state has changed.
		if (_upd->state()!=_state)
		{
			// Update messages and button in response to state change.
			_state=_upd->state();
			switch (_state)
			{
			case pkg::update::state_srclist:
			case pkg::update::state_download:
				_action.text("Downloading package information");
				break;
			case pkg::update::state_build_local:
			case pkg::update::state_build_sources:
				_action.text("Building list of packages");
				break;
			case pkg::update::state_done:
				_action.text("Done");
				_cancel_button.text("OK");
				delete _upd;
				_upd=0;
                tbx::app()->remove_idle_command(&_thread_runner);
				break;
			case pkg::update::state_fail:
				_action.text("Failed");
				_cancel_button.text("OK");
				//TODO: _error_field.text(string(_upd->message()));
				delete _upd;
				_upd=0;
                tbx::app()->remove_idle_command(&_thread_runner);
				break;
			}
		}
	}
}

/**
 * Poll the libpkg thread system which will run the
 * commit process and update the commit window
 */
void UpdateListWindow::PkgThreadRunner::execute()
{
	pkg::download::poll_all();
    pkg::thread::poll_all();
};

/**
 * Cancel commit or close dialog
 */
void UpdateListWindow::CancelCommand::execute()
{
	_me->_window.hide();
	_me->_instance = 0;
    tbx::app()->remove_idle_command(&(_me->_thread_runner));
	delete _me;
}

