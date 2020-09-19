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
