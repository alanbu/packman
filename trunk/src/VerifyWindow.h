/*********************************************************************
* Copyright 2012 Alan Buckley
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

#ifndef VERIFY_WINDOW_H
#define VERIFY_WINDOW_H

#include "Verify.h"
#include "tbx/displayfield.h"
#include "tbx/slider.h"
#include "tbx/command.h"

class VerifyWindow : public tbx::Command
{
   tbx::Window _window;
   tbx::DisplayField _status_text;
   tbx::Slider _package_progress;
   tbx::Slider _file_progress;
   tbx::CommandMethod<VerifyWindow> _do_cancel;
   Verify _verify;
   Verify::State _last_state;

public:
   VerifyWindow();
   virtual void execute();

private:
   void cancel();
};

#endif
