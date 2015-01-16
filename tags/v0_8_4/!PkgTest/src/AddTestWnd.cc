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
/*
 * AddTestWnd.cc
 *
 *  Created on: 19 Feb 2014
 *      Author: alanb
 */

#include "AddTestWnd.h"
#include "PkgTestWnd.h"

#include "tbx/application.h"
#include "tbx/actionbutton.h"
#include "tbx/messagewindow.h"
#include "tbx/path.h"

AddTestWnd::AddTestWnd(tbx::Object obj) :
   _window(obj),
   _name_field(_window.gadget(1)),
   _desc_field(_window.gadget(3)),
   _add_command(this, &AddTestWnd::add)
{
	tbx::ActionButton add_button(_window.gadget(5));
	add_button.add_selected_command(&_add_command);
	_window.add_about_to_be_shown_listener(this);
}

AddTestWnd::~AddTestWnd()
{
}

/**
 * Make sure fields are clear of previous data
 */
void AddTestWnd::about_to_be_shown(tbx::AboutToBeShownEvent &e)
{
	_name_field.text("");
	_desc_field.text("");
}

/**
 * Add the new test
 */
void AddTestWnd::add()
{
	TestInfo ti;
	ti.script = _name_field.text();
	ti.description = _desc_field.text();

	if (ti.script.empty() || ti.description.empty())
	{
		tbx::show_message("Must enter a name and description", "Add Test", "error");
	} else
	{
		PkgTestWnd *pkg_test = PkgTestWnd::from_window(_window.ancestor_object());
		TestList &tests = pkg_test->tests();
		if (tests.contains(ti.script))
		{
			tbx::show_message("A test with this name already exists", "Add Test", "error");
		} else
		{
			std::string script("<PkgTest$Dir>.Tests." + ti.script);
			// Don't overwrite if script is already in directory
			if (!tbx::Path(script).exists())
			{
				std::ofstream ofs(script.c_str());
				ofs << "--" << ti.description << std::endl;
				ofs.close();
			}
			tests.push_back(ti);
			tests.write("<PkgTest$Dir>.TestList");
			pkg_test->tests_updated();
			tbx::app()->os_cli("Filer_Run " + script);
			_window.hide();
		}
	}
}
