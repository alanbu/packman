/*********************************************************************
* Copyright 2012-2013 Alan Buckley
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

#include "VerifyWindow.h"
#include "VerifyFailedWindow.h"
#include "tbx/actionbutton.h"
#include "tbx/application.h"
#include "tbx/messagewindow.h"

VerifyWindow::VerifyWindow() :
   _window("Verify"),
   _status_text(_window.gadget(0)),
   _package_progress(_window.gadget(2)),
   _file_progress(_window.gadget(4)),
   _do_cancel(this, &VerifyWindow::cancel)
{
    tbx::ActionButton cancel = _window.gadget(5);
    cancel.add_selected_command(&_do_cancel);

    _window.show();
    tbx::app()->add_idle_command(this);
    _status_text.text("Building list of installed packages");
    _last_state = _verify.state();
}

/**
 * Polled when app is idle to process the verify
 */
void VerifyWindow::execute()
{
    _verify.poll();

    if (_verify.state() != _last_state)
    {
	    switch(_verify.state())
	    {
	       case Verify::NEXT_PACKAGE:
	          _status_text.text("Preparing to check next package");
	          if (_last_state == Verify::BUILD_PACKAGE_LIST)
	          {
	             _package_progress.upper_bound(_verify.package_total());
	          }
	          _package_progress.value(_verify.package_done());
	          break;

	      case Verify::BUILD_FILE_LIST:
	          {
	             std::string msg("Building file list for ");
	             msg += _verify.current();
	             _status_text.text(msg);
	          }
	          break;

		  case Verify::VERIFY_FILES:
		      _file_progress.upper_bound(_verify.file_total());
		      {
	              std::string msg("Verifying file list for ");
	              msg += _verify.current();
	              _status_text.text(msg);
	          }
	          break;

		case Verify::DONE:
		     tbx::app()->remove_idle_command(this);
		     if (!_verify.cancelled())
		     {
				 if (_verify.failed_total())
				 {
					new VerifyFailedWindow(_verify.failed_packages());
				 } else
				 {
					 tbx::show_message("All installed packages verified!","info");
				 }
		     }
		     _window.delete_object();
		     delete this;
		     return; // Mustn't touch any variables
		     break;

	       case Verify::BUILD_PACKAGE_LIST: break; // Should never get here
	    }

        _last_state = _verify.state();
	}

    if (_last_state == Verify::VERIFY_FILES)
    {
       _file_progress.value(_verify.file_done());
    }
}

/**
 * Cancel verify
 */
void VerifyWindow::cancel()
{
   _verify.cancel();
}
