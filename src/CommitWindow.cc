/*
 * CommitWindow.cc
 *
 *  Created on: 27-Mar-2009
 *      Author: alanb
 */

#include "CommitWindow.h"
#include "Packages.h"
#include "ConflictsWindow.h"
#include "CommitFailedWindow.h"

#include "tbx/application.h"
#include "tbx/deleteonhidden.h"


#include "libpkg/pkgbase.h"
#include "libpkg/download.h"

using namespace std;

CommitWindow *CommitWindow::_instance = 0;

/** The cost of one file operation.
 * This is a conversion factor for equating the cost of a file operation
 * (such as rename or delete) with the cost of moving a given number of
 * bytes.  It is necessarily an approximate, but a good choice will
 * noticibly improve the linearity of the progress bar. */

const unsigned int fileop_cost=16384;

CommitWindow::CommitWindow() :
    _window("Commit"),
    _action(_window.gadget(1)),
    _progress(_window.gadget(2)),
    _cancel_button(_window.gadget(3)),
    _cancel_command(this),
    _commit(0),
    _state(pkg::commit::state_done)
{
   _instance = this;
   _cancel_button.add_select_command(&_cancel_command);
   _cancel_button.fade(true);

   pkg::pkgbase *package_base = Packages::instance()->package_base();

   // Construct list of packages to be processed.
   std::set<string> packages;

   // For each entry in selstat, look to see whether the package
   // should be unpacked.  If so, add it to the list of packages
   // to be processed.

	for (std::map<string,pkg::status>::const_iterator i=
			package_base->selstat().begin();
                i != package_base->selstat().end();
                ++i)
	{
		const std::string& pkgname=i->first;
		const pkg::status& selstat=i->second;
		const pkg::status& curstat=package_base->curstat()[pkgname];
		if (unpack_req(curstat,selstat))
			packages.insert(pkgname);
	}

	// For each entry in curstat, look to see whether the package
	// should be removed.  If so, add it to the list of packages
	// to be processed.
	for (std::map<std::string,pkg::status>::const_iterator i=
		package_base->curstat().begin();
           i!=package_base->curstat().end();++i)
	{
		const string& pkgname=i->first;
		const pkg::status& curstat=i->second;
		const pkg::status& selstat=package_base->selstat()[pkgname];
		if (remove_req(curstat,selstat))
			packages.insert(pkgname);
	}

	// Begin new commit operation.
	_commit = new pkg::commit(*package_base, packages);

	// Start libpkg threads
	tbx::app()->add_idle_command(&_thread_runner);

    _window.show();
}

CommitWindow::~CommitWindow()
{
	delete _commit;
}

/**
 * Update UI with current state of commit
 */
void CommitWindow::poll()
{
	if (_commit)
	{
		// Update progress bar.
		// (Must do this before checking state, in case the commit
		// operation is deleted.)
		double done =
		 (double)(_commit->bytes_done() + _commit->files_done() * fileop_cost) /
             (double)(_commit->bytes_total() + _commit->files_total() * fileop_cost);

         _progress.value((int)(done * 100));

		// Check whether commit operation state has changed.
		if (_commit->state() != _state)
		{
			// Update messages and button in response to state change.
			_state=_commit->state();
			switch (_state)
			{
			case pkg::commit::state_pre_download:
				_action.text("Preparing for downloading");
				break;
			case pkg::commit::state_download:
				_action.text("Downloading");
				break;
			case pkg::commit::state_unpack:
				_action.text("Unpacking / Removing");
				break;
			case pkg::commit::state_configure:
				_action.text("Configuring");
				break;
			case pkg::commit::state_purge:
				_action.text("Purging");
				break;
			case pkg::commit::state_update_sysvars:
				_action.text("Updating system variables");
				break;

			case pkg::commit::state_update_sprites:
				_action.text("Updating sprite pool");
				break;

			case pkg::commit::state_done:
                _progress.value(100);
				_action.text("Done");
				_cancel_button.text("Close");
				_cancel_button.fade(false);
				delete _commit;
				_commit=0;
                tbx::app()->remove_idle_command(&_thread_runner);
                Packages::instance()->unset_upgrades_available();
				break;

			case pkg::commit::state_fail:
				{
					std::string last_action = _action.text();
					_action.text("Failed ");
					new CommitFailedWindow(_commit, last_action);
					_cancel_button.text("Close");
					_cancel_button.fade(false);
					if (_commit->files_that_conflict().size())
						open_conflicts_window();
					delete _commit;
					_commit=0;
					tbx::app()->remove_idle_command(&_thread_runner);
					// Defer delete to idle
					tbx::app()->add_idle_command(&_cancel_command);
				}
				break;
			}
		}
	}
}

/**
 * Closes commit window.
 *
 * Commit should have been completed before this is run
 */
void CommitWindow::close()
{
	if (_commit == 0)
	{
		_window.hide();
		_instance = 0;
		delete this;
	}
}

/**
 * Check if it is OK to start a new commit
 *
 * Closes commit window if it is not running but the
 * window is still showing.
 *
 * Shows a message if the commit window is running.
 *
 * @returns true if it is OK to run a commit
 */
bool CommitWindow::ok_to_run()
{
	if (showing())
	{
		CommitWindow *cw = CommitWindow::instance();
		// If it's showing at the end of a completed operation
		// we can close it.
		if (cw->done()) cw->close();
		else
		{
			tbx::Window only_one("OnlyOne");
			only_one.add_has_been_hidden_listener(new tbx::DeleteObjectOnHidden());
			return false;
		}
	}
	return true;
}


/**
 * Poll the libpkg thread system which will run the
 * commit process and update the commit window
 */
void CommitWindow::PkgThreadRunner::execute()
{
	pkg::download::poll_all();
    pkg::thread::poll_all();
};

/**
 * Cancel commit or close dialog
 */
void CommitWindow::CancelCommand::execute()
{
	_me->_window.hide();
	_me->_instance = 0;
    tbx::app()->remove_idle_command(&(_me->_thread_runner));
    // In case we were cancelled from poll loop
    tbx::app()->remove_idle_command(&(_me->_cancel_command));
	delete _me;
}

void CommitWindow::open_conflicts_window()
{
	new ConflictsWindow(_commit->files_that_conflict());
}
