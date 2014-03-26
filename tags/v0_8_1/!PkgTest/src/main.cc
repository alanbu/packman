/*********************************************************************
* Copyright 2014 Alan Buckley
*
* This file is part of PkgTest (PackMan unit testing)
*
* PackMan is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* PkgTest is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with PackMan. If not, see <http://www.gnu.org/licenses/>.
*
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
