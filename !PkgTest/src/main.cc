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

#include "PkgTestWnd.h"
#include "AddTestWnd.h"

#include "tbx/application.h"
#include "tbx/autocreatelistener.h"
#include "tbx/matchlifetime.h"

using namespace std;

int main()
{
   tbx::Application my_app("<PkgTest$Dir>");

   PkgTestWnd window_handler;
   tbx::MatchLifetime<AddTestWnd> mlt_add_test("AddTest");

   my_app.set_autocreate_listener("Window", &window_handler);

   my_app.run();

   return 0;
}
