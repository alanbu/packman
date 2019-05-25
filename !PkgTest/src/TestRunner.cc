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
 * TestRunner.cc
 *
 *  Created on: 6 Feb 2014
 *      Author: alanb
 */

#include "TestRunner.h"
#include "Packages.h"
#include "CommitWindow.h"

TestRunner *TestRunner::_instance = 0;

#include "luacc/lua.h"
#include "luacc/lualib.h"
#include "luacc/lauxlib.h"

#include "tbx/application.h"
#include "tbx/path.h"
#include "tbx/hourglass.h"

#include <ctime>
#include <sstream>
#include <iomanip>
#include <stdexcept>

// Libraries added for PkgTest
extern void loadlib_test(lua_State *L);
extern void loadlib_pkg(lua_State *L);

// Recursive directory delete from TestLibg.cc
extern void recursive_delete(const std::string &dirname);

/**
 * Return the timestamp for log entries
 */
std::string timestamp()
{
	std::time_t now;
	std::time(&now);
	struct std::tm *tm_now = std::localtime(&now);
	std::ostringstream ss;
	ss << std::setfill('0')
	   << std::setw(2) << tm_now->tm_hour
	   << ":" << std::setw(2) << tm_now->tm_min
	   << ":" << std::setw(2) << tm_now->tm_sec
	   << " ";
	return ss.str();
}

/**
 * Add timestamp to log and return the stream so more
 * information can be added to it.
 */
std::ostream &test_log()
{
	std::ostream &log = tr()->log();
	log << timestamp();
	return log;
}

TestRunner::TestRunner() :
			_listener(0),
			_test_index(0),
			_errors(0),
			_waiting(false),
			_cancel(false),
			_paused(false),
			_package_log(false)
{
	_poll_method = &TestRunner::start_up;
	_instance = this;
}

TestRunner::~TestRunner()
{
	_instance = 0;
}

/**
 * Add all the tests from a test list
 */
void TestRunner::add_tests(const TestList &tests)
{
	_tests = tests;
}

/**
 * Call on idle to keep the runner going
 *
 * @returns true if needs to be called again
 */
bool TestRunner::poll()
{
	return (this->*_poll_method)();
}

/**
 * Start up test system
 */
bool TestRunner::start_up()
{
	std::time_t now;
	std::time(&now);
	struct std::tm *tm_now = std::localtime(&now);

	// Setup logs
	std::string logs_path = "<PkgTest$Dir>.Logs";
	tbx::Path(logs_path).create_directory();

	std::ostringstream ss;
	ss << logs_path << ".L"
			<< std::setfill('0')
	        << (tm_now->tm_year - 1900)
	        << std::setw(2)
	        << tm_now->tm_mon
	        << tm_now->tm_mday
			<< tm_now->tm_hour
			<< std::setw(2) << tm_now->tm_min
			<< std::setw(2) << tm_now->tm_sec;
	_log_path = ss.str();
	tbx::Path(_log_path).create_directory();

	_run_log.open((_log_path + ".!RunLog").c_str());
	_run_log << timestamp() << "Starting running "
			<< _tests.size() << " tests" << std::endl;
	if (_tests.size() == 0)
	{
		_run_log << timestamp() << "No tests to run" << std::endl;
		if (_listener) _listener->tests_init_failed();
		return false;
	}

	_errors = 0;
	std::string pb_errmsg;
	try
	{
		Packages::instance()->ensure_package_base();
	} catch(std::exception &e)
	{
		pb_errmsg = e.what();
	} catch(...)
	{
		pb_errmsg = "Non standard exception";
	}

	if (!pb_errmsg.empty())
	{
		_run_log << timestamp() << "Failed to set up test packaging system: "
				<< pb_errmsg << std::endl;
		_run_log.close();
		if (_listener) _listener->tests_init_failed();
		return false;
	}

	if (_listener) _listener->tests_started(_tests.size());

	_poll_method = &TestRunner::run_test;

	return true;
}

/**
 * Do nothing polling used to ensure
 * we don't get a callback when we don't want one.
 */
bool TestRunner::do_nothing()
{
	return true;
}

/**
 * Run a single test
 */
bool TestRunner::run_test()
{
	_poll_method = &TestRunner::do_nothing;

	std::string script = _tests[_test_index].script;
	_run_log << timestamp() << "Starting " << script
			<< _tests[_test_index].description << std::endl;

	lua_State* lua_state;
	lua_state = lua_open();
	luaL_openlibs (lua_state);
	loadlib_test(lua_state);
	loadlib_pkg(lua_state);

	std::string log_file = _log_path + "." + script;

	_log.open(log_file.c_str());
	_log << timestamp() << "Test started" << std::endl;

	std::string test_file = "<PkgTest$Dir>.tests."+ script;
	if (_listener) _listener->test_started(_tests[_test_index]);
	tbx::app()->yield();

	try
	{
		if (luaL_dofile (lua_state, test_file.c_str()))
		{
			const char *msg = lua_tostring(lua_state, -1);
			std::string err = msg ? msg : "Unknown error";
			throw std::runtime_error(err);
		} else
		{
			_log << timestamp() << "Test succeeded" << std::endl;
			_run_log << timestamp() << "Test " << script << " succeeded"<< std::endl;
			if (_listener) _listener->test_succeeded();
		}
	} catch(std::exception &e)
	{
		_log << timestamp() << "Test failed: " << e.what() << std::endl;
		_run_log << timestamp() << "Test " << script << " failed: " << e.what() << std::endl;
	    if (_listener) _listener->test_failed(e.what());
		_errors++;
	}

	_run_log << "Before lua close" << std::endl;
	lua_close(lua_state);
	_run_log << "After lua close" << std::endl;

	_log.close();

	_run_log << "Checking for another test " << _test_index << " " << _tests.size() << std::endl;

	if (_cancel || ++_test_index == _tests.size())
	{
		_run_log << "tests should be finished" << std::endl;
		_poll_method = &TestRunner::tests_finished;
	} else
	{
		_run_log << "starting another test" << std::endl;
		_poll_method = &TestRunner::run_test;
	}
	return true;
}

/**
 * All tests have finished
 */
bool TestRunner::tests_finished()
{
	_poll_method = &TestRunner::do_nothing;

	_run_log << "In tests finished" << std::endl;
	if (_cancel) _run_log << timestamp() << "Tests were stopped before completion" << std::endl;
	else _run_log << timestamp() << "Finished running tests" << std::endl;
	if (_listener) _listener->tests_finished(_errors);

	return false;
}

/**
 * Display a message
 */
void TestRunner::message(const char *msg)
{
	if (_listener) _listener->message(msg);
	_log << timestamp() << msg << std::endl;
}

/**
 * Run the package commit using a slightly modified
 * version of the PackMan commit window.
 *
 * @throws std::runtime_error if it fails
 */
void TestRunner::commit()
{
	if (CommitWindow::showing()) throw std::runtime_error("A commit is in progress");

	Packages::instance()->logging(_package_log);

	CommitWindow *cw = new CommitWindow();
	_waiting = true;
	_waiting_error.clear();

	while (_waiting && tbx::app()->yield()); // No body needed

	if (!tbx::app()->running())
	{
		_cancel = true;
		throw std::runtime_error("Application has been quit");
	}

	cw->close();

	if (!_waiting_error.empty())
	{
		// Window is closed by CommitWindow
		std::string msg;
		if (_expected_failure.empty()) msg = "Commit failed ";
		else msg = "Commit ";

		throw std::runtime_error(msg + _waiting_error);
	}
}

/**
 * Run a commit which is expected to fail
 *
 * @param name of expected failure reason
 */
void TestRunner::commit_fail(std::string reason)
{
	_expected_failure = reason;
	commit();
	_expected_failure.clear();
}

/**
 * Callback from commit window when commit has succeeded
 *
 * @param commit pkg::commit for successful commit
 */
void TestRunner::commit_succeeded(pkg::commit *commit)
{
	add_package_log();
	if (commit->warnings())
	{
		pkg::log *w = commit->warnings();
		for (pkg::log::const_iterator e = w->begin(); e != w->end(); ++e)
		{
			_log << e->when() << " Warning: " << e->text() << std::endl;
		}
		_log << timestamp() << "Commit succeeded with warnings" << std::endl;
	} else
	{
		_log << timestamp() << "Commit succeeded" << std::endl;
	}

	if (!_expected_failure.empty())
	{
		_waiting_error = "succeeded when it should have failed";
	}
	_waiting = false;
}

/**
 * Callback from commit window when commit has failed
 *
 * @param commit pkg::commit for failed commit
 * @param action action being run when commit failed
 */
void TestRunner::commit_failed(pkg::commit *commit, const std::string &action)
{
	add_package_log();

	std::string reason;

	if (commit->files_that_conflict().size())
	{
		string ts = timestamp();
		for (std::set<std::string>::const_iterator c = commit->files_that_conflict().begin();
			c != commit->files_that_conflict().end(); ++c)
		{
			_log << ts << "Conflict: " << *c << std::endl;
		}
		_waiting_error = "conflicting files";
		reason = "conflicts";
	} else
	{
		_log << timestamp() << "Error: " << commit->message() << std::endl;
		_waiting_error = "during " + action;
		reason = commit->message();
	}

	if (!_expected_failure.empty())
	{
		// Expecting a failure
		if (matches(reason, _expected_failure))
		{
			// Fails as expected
			_waiting_error.clear();
		} else
		{
			_waiting_error = "failed for the wrong reason '" + reason + "' " + _waiting_error;
		}
	}
	_waiting = false;
}

/**
 * Check if string matches a simple wild carded pattern
 *
 * Wild card character is "*" for 0 or more characters
 *
 * @param what string to check
 * @param pattern pattern to check against
 * @return true if it matches
 */
bool TestRunner::matches(const std::string &what, const std::string &pattern)
{
	std::string::size_type pos = pattern.find('*');
	bool match = (pos != std::string::npos);
	if (!match)
	{
		// Pattern has no wild cards so do exact match
		match = (what == pattern);
	} else
	{
		std::string::size_type pattern_pos = pos+1;
		std::string::size_type what_pos = 0;

		// Compare anything before first wild card
		if (pos > 0)
		{
			if (what.compare(0, pos, pattern, 0, pos) != 0) match = false;
			what_pos += pos;
		}
		pos = pattern.find('*', pattern_pos);
		// Compare strings between wild cards
		while (match && pos != std::string::npos)
		{
			std::string find = pattern.substr(pattern_pos, pos - pattern_pos);
			what_pos = what.find(find, what_pos);
			if (what_pos == std::string::npos)
			{
				match = false;
			} else
			{
				what_pos += pos - pattern_pos;
				pattern_pos = pos + 1;
				pos = pattern.find('*', pattern_pos);
			}
		}

		// Check anything left over
		if (match && pattern_pos < pattern.size())
		{
			std::string::size_type len = pattern.size() - pattern_pos;
			match = (what.compare(pattern_pos, len, pattern, what.size() - len, len) == 0);
		}
	}

	return match;
}

/**
 * Toggle the paused state
 */
void TestRunner::toggle_pause(const std::string msg)
{
	if (_paused) resume();
	else pause(msg);
}

/**
 * Pause the test runner
 */
void TestRunner::pause(const std::string msg)
{
	if (_listener) _listener->paused(msg);

	_paused =  true;
	bool (TestRunner::*current_method)() = _poll_method;
	_poll_method = &TestRunner::do_nothing;

	while (_paused && tbx::app()->yield()); // No body needed
	if (!tbx::app()->running())
	{
		_cancel = true;
		throw std::runtime_error("Application has been quit");
	}

	_paused = false;
	if (_listener) _listener->resumed();
	_poll_method = current_method;
}

/**
 * Continue after a pause
 */
void TestRunner::resume()
{
	_paused = false;
}

/**
 * Add the package log to the current run log if flag is set
 */
void TestRunner::add_package_log()
{
	if (!_package_log) return;

	pkg::log *pkg_log = Packages::instance()->current_log().get();
	for (pkg::log::const_iterator e = pkg_log->begin(); e != pkg_log->end(); ++e)
	{
		_log << e->when_text() << " PkgLog: " << e->text() << std::endl;
	}
}

/**
 * Build the test disc image, rebuilding it if it already exists
 */
void TestRunner::rebuild_disc()
{
	tbx::Hourglass hg;

	tbx::Path disc_path("<PkgTestDisc$Dir>");
	if (disc_path.exists()) recursive_delete(disc_path);
	tbx::Path master_path("<PkgTest$Dir>.MasterDisc");
	master_path.copy(disc_path, tbx::Path::COPY_RECURSE);
	Packages::instance()->reset_package_base();
}
