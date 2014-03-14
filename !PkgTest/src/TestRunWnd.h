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
 * TestRunWnd.h
 *
 *  Created on: 11 Feb 2014
 *      Author: alanb
 */

#ifndef TESTRUNWND_H_
#define TESTRUNWND_H_

#include "TestRunner.h"
#include "tbx/window.h"
#include "tbx/command.h"
#include "tbx/scrolllist.h"
#include "tbx/displayfield.h"
#include "tbx/actionbutton.h"
#include "tbx/slider.h"

/**
 * Window to run the tests and report on there progress
 */
class TestRunWnd :
		public tbx::Command,
		public TestRunnerListener
{
	tbx::Window _window;
	tbx::ScrollList _tests_list;
	tbx::DisplayField _current_test;
	tbx::DisplayField _message;
	tbx::DisplayField _tests_run;
	tbx::DisplayField _errors_display;
	tbx::ActionButton _pause_button;
	tbx::Slider _run_progress;
	tbx::Slider _error_progress;

	int _tests;
	int _errors;

	tbx::CommandMethod<TestRunWnd> _pause_command;
	tbx::CommandMethod<TestRunWnd> _view_log_command;
	tbx::CommandMethod<TestRunWnd> _edit_script_command;
	std::string _pause_old_msg;
	std::string _log_directory;

	void test_done();
	void change_test_sprite(const std::string &sprite_name);

public:
	TestRunWnd();
	virtual ~TestRunWnd();

    virtual void execute();

    // Button commands
    void pause();
    void view_log();
    void edit_script();

    // Test runner callbacks
    virtual void tests_init_failed();
    virtual void tests_started(int num_tests);
    virtual void tests_finished(int errors);

    virtual void test_started(const TestInfo &test);
    virtual void test_failed(std::string msg);
    virtual void test_succeeded();
	virtual void message(const char *msg);

	virtual void paused(const std::string &msg);
	virtual void resumed();

};

#endif /* TESTRUNWND_H_ */
