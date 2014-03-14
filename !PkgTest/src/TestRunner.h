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
 * TestRunner.h
 *
 *  Created on: 6 Feb 2014
 *      Author: alanb
 */

#ifndef TESTRUNNER_H_
#define TESTRUNNER_H_

#include "TestInfo.h"
#include <fstream>

namespace pkg
{
	class commit;
};
/**
 * Call back interface from the test runner
 */
class TestRunnerListener
{
public:
	virtual ~TestRunnerListener() {}

    virtual void tests_init_failed() = 0;
    virtual void tests_started(int num_tests) = 0;
    virtual void tests_finished(int errors) = 0;

    virtual void test_started(const TestInfo &test) = 0;
    virtual void test_failed(std::string msg) = 0;
    virtual void test_succeeded() = 0;

	virtual void message(const char *msg) = 0;
	virtual void paused(const std::string &msg) = 0;
	virtual void resumed() = 0;
};

/**
 * Class to run the tests
 */
class TestRunner
{
	static TestRunner *_instance;
	TestRunnerListener *_listener;
	bool (TestRunner::*_poll_method)();

	TestList _tests;
	unsigned int _test_index;
	std::string _log_path;
	std::ofstream _run_log;
	std::ofstream _log;
	int _errors;

	bool _waiting;
	std::string _waiting_error;
	std::string _expected_failure;
	bool _cancel;
	bool _paused;
	bool _package_log;
public:
	TestRunner();
	virtual ~TestRunner();

	static TestRunner *instance() {return _instance;}

	void rebuild_disc();
	void add_tests(const TestList &tests);
	const TestList &tests() const {return _tests;}
	const std::string &log_path() const {return _log_path;}

	void package_log(bool on) {_package_log = on;}

	/**
	 * Set listener to report information back from the runner
	 */
	void set_listener(TestRunnerListener *listener) {_listener = listener;}
	bool poll();

	// Poll methods
	bool start_up();
	bool run_test();
	bool do_nothing();
	bool tests_finished();

	void message(const char *msg);

	std::ostream &log() {return _log;}

	void add_package_log();

	void commit();
	void commit_fail(std::string reason);

	// Callbacks from commit window
	void commit_succeeded(pkg::commit *commit);
	void commit_failed(pkg::commit *commit, const std::string &action);

	void toggle_pause(const std::string msg);
	void pause(const std::string msg);
	void resume();

	// Helpers
	bool matches(const std::string &what, const std::string &pattern);

};

/**
 * Get current test runner
 */
inline TestRunner *tr() {return TestRunner::instance();}

// Convenience method to get timestamped test log
std::ostream &test_log();

#endif /* TESTRUNNER_H_ */
