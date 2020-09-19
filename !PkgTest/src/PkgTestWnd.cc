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
#include "TestRunWnd.h"

#include "tbx/window.h"
#include "tbx/menu.h"
#include "tbx/application.h"
#include "tbx/actionbutton.h"
#include "tbx/messagewindow.h"
#include "tbx/path.h"

#include <sstream>

using namespace tbx;

PkgTestWnd::PkgTestWnd() :
		_run_all_command(this, &PkgTestWnd::run_all),
		_run_selected_command(this, &PkgTestWnd::run_selected),
		_edit_script_command(this, &PkgTestWnd::edit_script),
		_update_menu(this)
{
}
/**
 * Get the PkgTestWnd object from the window handle
 */
PkgTestWnd *PkgTestWnd::from_window(tbx::Window w)
{
	return reinterpret_cast<PkgTestWnd *>(w.client_handle());
}


/**
 * Set up listeners and commands for the window.
 */
void PkgTestWnd::auto_created(std::string template_name, Object object)
{
    Window window(object);
    window.client_handle(this);

    Menu menu = window.menu();
    menu.add_about_to_be_shown_listener(&_update_menu);

    // Commands
    window.add_command(1, &_edit_script_command);

    ActionButton run_all(window.gadget(1));
    run_all.add_selected_command(&_run_all_command);
    ActionButton run_selected(window.gadget(4));
    run_selected.add_selected_command(&_run_selected_command);

    _tests_list = window.gadget(0);
    _tests.read("<PkgTest$Dir>.TestList");

    _add_pkg_log = window.gadget(3);
    _rebuild_disc = window.gadget(6);

    if (!tbx::Path("<PkgTestDisc$Dir>").exists())
    {
    	// Must rebuild disc if it doesn't exist
    	_rebuild_disc.on(true);
    	_rebuild_disc.fade(true);
    }
    tests_updated();
}

/**
 * Repopulate window when tests have been changed
 */
void PkgTestWnd::tests_updated()
{
	_tests_list.clear();
    for (TestList::iterator t = _tests.begin(); t != _tests.end(); ++t)
    {
    	std::string test(t->script);
    	test+=" ";
    	test+=t->description;
    	_tests_list.add_item(test);
    }
}


/**
 * Update the menus
 */
void PkgTestWnd::UpdateMenu::about_to_be_shown(tbx::AboutToBeShownEvent &event)
{
	 Menu menu(event.id_block().self_object());
	 MenuItem test_entry = menu.item(1);
	 int idx = _me->_tests_list.first_selected();
	 if (idx == -1)
	 {
		 test_entry.fade(true);
		 test_entry.text("Test ''");
	 } else
	 {
		 test_entry.fade(false);
		 std::string text("Test '");
		 text += _me->_tests[idx].script;
		 text += "'";
		 test_entry.text(text);
	 }
}


/**
 * Run all tests
 */
void PkgTestWnd::run_all()
{
	if (TestRunner::instance())
	{
		tbx::show_message("Tests already running");
	} else
	{
		TestRunner *runner = new TestRunner();
		if (_rebuild_disc.on()) runner->rebuild_disc();
		runner->add_tests(_tests);
		runner->package_log(_add_pkg_log.on());
		// Creating the TestRunWnd runs the tests
		new TestRunWnd();
	}
}

/**
 * Run selected tests
 */
void PkgTestWnd::run_selected()
{
	int idx = _tests_list.first_selected();
	if (idx == -1)
	{
		tbx::show_message("No test selected to run");
	} else	if (TestRunner::instance())
	{
		tbx::show_message("Tests already running");
	} else
	{
		TestRunner *runner = new TestRunner();
		TestList to_run;
		while (idx != -1)
		{
			to_run.push_back(_tests[idx]);
			idx = _tests_list.next_selected(idx);
		}
		if (_rebuild_disc.on()) runner->rebuild_disc();
		runner->add_tests(to_run);
		runner->package_log(_add_pkg_log.on());
		// Creating the TestRunWnd runs the tests
		new TestRunWnd();
	}
}

/**
 * Edit the selected script
 */
void PkgTestWnd::edit_script()
{
	int idx = _tests_list.first_selected();
	if (idx != -1)
	{
		tbx::app()->os_cli("Filer_Run <PkgTest$Dir>.Tests." + _tests[idx].script);
	}
}
