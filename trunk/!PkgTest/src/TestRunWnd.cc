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
 * TestRunWnd.cc
 *
 *  Created on: 11 Feb 2014
 *      Author: alanb
 */

#include "TestRunWnd.h"
#include "LogWnd.h"

#include "tbx/application.h"
#include "tbx/stringutils.h"
#include "tbx/messagewindow.h"
#include <stdexcept>

TestRunWnd::TestRunWnd() :
	_window("TestRun"),
	_tests_list(_window.gadget(1)),
	_current_test(_window.gadget(3)),
	_message(_window.gadget(11)),
	_tests_run(_window.gadget(5)),
	_errors_display(_window.gadget(9)),
	_pause_button(_window.gadget(12)),
	_run_progress(_window.gadget(15)),
	_error_progress(_window.gadget(16)),
	_tests(0),
	_errors(0),
	_pause_command(this, &TestRunWnd::pause),
	_view_log_command(this, &TestRunWnd::view_log),
	_edit_script_command(this, &TestRunWnd::edit_script)
{
	TestRunner *runner = TestRunner::instance();
	if (runner == 0) throw std::logic_error("TestRunner must be created before the test run window");

	for(TestList::const_iterator t = runner->tests().begin(); t != runner->tests().end(); ++t)
	{
		_tests_list.add_item(t->script + " " + t->description);
	}

	_run_progress.upper_bound(runner->tests().size());
	_error_progress.upper_bound(runner->tests().size());

	_message.text("Setting up tests");
	_pause_button.add_selected_command(&_pause_command);

	tbx::ActionButton view_log(_window.gadget(13));
	view_log.add_selected_command(&_view_log_command);

	tbx::ActionButton edit_script(_window.gadget(14));
	edit_script.add_selected_command(&_edit_script_command);

    runner->set_listener(this);
	tbx::app()->add_idle_command(this);
}

TestRunWnd::~TestRunWnd()
{
}

/**
 * Polling command used to keep the testrunner going
 */
void TestRunWnd::execute()
{
	TestRunner *runner = TestRunner::instance();
	if (runner)
    {
	   if (!runner->poll())
	   {
		  tbx::app()->remove_idle_command(this);
		  delete runner;

		  // Enable close button
		  _window.gadget(17).fade(false);
		  _pause_button.fade(true);
	   }
    }
}


/**
 * Callback from test runner with a message
 */
void TestRunWnd::message(const char *msg)
{
	_message.text(msg);
}

void TestRunWnd::tests_init_failed()
{
	_message.text("Tests initialisation failed");
}
void TestRunWnd::tests_started(int num_tests)
{
	_message.text("Tests started");
	tbx::DisplayField total_tests = _window.gadget(7);
	total_tests.text(tbx::to_string(num_tests));
	_log_directory = tr()->log_path();
}

void TestRunWnd::tests_finished(int errors)
{
	std::ostringstream ss;
	ss << "Tests finished " << errors << " error(s)";
	_message.text(ss.str());
}

void TestRunWnd::test_started(const TestInfo &test)
{
	_message.text("Started " + test.script);
	_current_test.text(test.description);
	change_test_sprite("running");
}

void TestRunWnd::test_failed(std::string msg)
{
	_message.text("Test failed: " + msg);
	_errors++;
	_errors_display.text(tbx::to_string(_errors));
	_error_progress.value(_errors);
	change_test_sprite("failed");
	test_done();
}

void TestRunWnd::test_succeeded()
{
	_message.text("Test succeeded");
	change_test_sprite("succeeded");
	test_done();
}

void TestRunWnd::change_test_sprite(const std::string &sprite_name)
{
	std::string text = _tests_list.item_text(_tests);
	_tests_list.add_item(text, tbx::app()->sprite_area()->get_sprite(sprite_name), _tests);
	_tests_list.delete_item(_tests+1);
}
/**
 * Update display and test count after a test
 * has finished
 */
void TestRunWnd::test_done()
{
	_tests++;
	_current_test.text("");
	_tests_run.text(tbx::to_string(_tests));
	_run_progress.value(_tests);
}

/**
 * Pause or resume current test
 */
void TestRunWnd::pause()
{
	tr()->toggle_pause("User pause");
}

/**
 * Test runner has been paused
 */
void TestRunWnd::paused(const std::string &msg)
{
	_pause_button.text("Continue");
	_pause_old_msg = _message.text();
	_message.text("** " + msg + " **");
}

/**
 * Test runner has been resumed after a pause
 */
void TestRunWnd::resumed()
{
	_pause_button.text("Pause");
	_message.text(_pause_old_msg);
}

/**
 * Show log for selected line
 */
void TestRunWnd::view_log()
{
	int idx = _tests_list.first_selected();
	if (idx == -1)
	{
		tbx::show_message("Select a test to see the log from");
	} else if (idx >= _tests)
	{
		tbx::show_message("Selected test has not been run yet");
	} else
	{
		std::string test = _tests_list.item_text(idx);
		std::string::size_type pos = test.find(' ');
		new LogWnd(_log_directory + "." + test.substr(0, pos));
	}
}

/**
 * Edit script for selected line
 */
void TestRunWnd::edit_script()
{
	int idx = _tests_list.first_selected();
	if (idx == -1)
	{
		tbx::show_message("Select a test to see the log from");
	} else
	{
		std::string test = _tests_list.item_text(idx);
		std::string::size_type pos = test.find(' ');
		std::string script = "<PkgTest$Dir>.Tests." + test.substr(0, pos);
		tbx::app()->os_cli("Filer_Run " + script);
	}
}

