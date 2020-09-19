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
 * UpdateListWindow.cc
 *
 *  Created on: 23-Jul-2009
 *      Author: alanb
 */

#include "UpdateListWindow.h"
#include "Packages.h"
#include "ErrorWindow.h"
#include "LogViewer.h"
#include "Choices.h"
#include "tbx/application.h"
#include "libpkg/download.h"
#include "libpkg/log.h"
#include <algorithm>
#include <iterator>

UpdateListWindow *UpdateListWindow::_instance = 0;

UpdateListWindow::UpdateListWindow() :
    _window("UpdateList"),
    _action(_window.gadget(1)),
    _progress(_window.gadget(2)),
    _cancel_button(_window.gadget(3)),
    _whats_new(_window.gadget(5)),
    _upgrade_all(_window.gadget(4)),
    _cancel_command(this),
    _upd(0),
    _state(pkg::update::state_done),
    _log_viewer(0),
    _show_log_command(this, &UpdateListWindow::show_log),
    _listener(nullptr)
{
   _instance = this;
   _cancel_button.add_select_command(&_cancel_command);
   _cancel_button.fade(true);

   _whats_new.add_select_command(&_show_whats_new);

   pkg::pkgbase *package_base = Packages::instance()->package_base();

   _upd=new pkg::update(*package_base);

	// Start libpkg threads
	tbx::app()->add_idle_command(&_thread_runner);

	// Set up list of packages before update
	const std::vector<PackageKey> &package_list = Packages::instance()->package_list();
	std::transform(package_list.begin(), package_list.end(),
			std::inserter(_whats_old, _whats_old.end()),
			[](const PackageKey &key) -> std::string { return key.pkgname; }
	);

	if (Packages::instance()->logging())
	{
	    _show_log  = _window.gadget(6);
		_log = Packages::instance()->new_log();
		_upd->log_to(_log.get());
		_show_log.add_select_command(&_show_log_command);
		_show_log.fade(true);
	} else
	{
		// Remove log button
		_window.remove_gadget(6);
	}

    _window.show();
}

UpdateListWindow::~UpdateListWindow()
{
	delete _upd;
	_instance = nullptr;
}

/**
 * Set listener for update lists finishing.
 *
 * @param listener - set to null to clear
 */
void UpdateListWindow::listener(UpdateListListener *listener)
{
	_listener = listener;
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
		if (denominator != pkg::update::npos)
		{
			while (denominator>0xfff)
			{
				numerator>>=1;
				denominator>>=1;
			}
			_progress.value(numerator * 100 / ((denominator)?denominator:1));
		}

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
				_cancel_button.text("Close");
			    _cancel_button.fade(false);
			    if (!_show_log.null()) _show_log.fade(false);
				delete _upd;
				_upd=0;
                tbx::app()->remove_idle_command(&_thread_runner);
                Packages::instance()->reset_package_list();
                Packages::instance()->unset_upgrades_available();
                if (Packages::instance()->upgrades_available())
                {
                	_upgrade_all.fade(false);
                }
                _whats_new.fade(!write_whats_new());
                if (_listener) _listener->lists_updated(true);
				break;
			case pkg::update::state_fail:
				_action.text("Failed");
				_cancel_button.text("Close");
			    _cancel_button.fade(false);
			    if (!_show_log.null()) _show_log.fade(false);
				new ErrorWindow(_upd->message(), "Failed to update package list(s)");
				delete _upd;
				_upd=0;
                tbx::app()->remove_idle_command(&_thread_runner);
                if (_listener) _listener->lists_updated(false);
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

/**
 * Show (or reshow) the log
 */
void UpdateListWindow::show_log()
{
	if (_log_viewer == 0) _log_viewer = new LogViewer(_log);
	_log_viewer->show();
}

/**
 * Write packages that are new with this update to a file
 *
 * @return true if there were any new packages
 */
bool UpdateListWindow::write_whats_new()
{
	const std::vector<PackageKey> &package_list = Packages::instance()->package_list();
	std::set<std::string> whats_new;
	for (std::vector<PackageKey>::const_iterator i = package_list.begin();
			i != package_list.end(); ++i)
	{
		const std::string &pkgname = i->pkgname;
		if (_whats_old.find(pkgname) == _whats_old.end())
		{
			whats_new.insert(pkgname);
		}
	}

	bool new_stuff = !whats_new.empty();
	if (new_stuff) new_stuff = Choices::ensure_choices_dir();
	if (new_stuff)
	{
		std::string wnpath(choices_write_path("WhatsNew"));
		std::ofstream wn(wnpath.c_str(), std::ios_base::out | std::ios_base::trunc);
		if (wn)
		{
			std::ostream_iterator<std::string> out_it(wn, " ");
			std::copy(whats_new.begin(), whats_new.end(), out_it);
		} else
		{
			new_stuff = false;
		}
	}

	return new_stuff;
}

/**
 * Check if the update list window is running
 *
 * As part of the check it will close the update list window if it is
 * not running but the window is still showing.
 *
 * @returns true if the update list window is running
 */
bool UpdateListWindow::running()
{
	if (showing())
	{
		if (_instance && _instance->_upd)
	    {
			return true;
	    } else
	    {
			_instance->_window.hide();
			delete _instance;
		}
	}
	return false;
}
