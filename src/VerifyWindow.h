/*********************************************************************
* This file is part of PackMan
*
* Copyright 2012-2020 AlanBuckley
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
   virtual ~VerifyWindow() {}
   virtual void execute();

private:
   void cancel();
};

#endif
