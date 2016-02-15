/*********************************************************************
* Copyright 2016 Alan Buckley
*
* This file is part of PackMan.
*
* PackMan is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* PackMan is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with PackMan. If not, see <http://www.gnu.org/licenses/>.
*
*****************************************************************************/
/*
 * PMTriggerRun.cc
 *
 *  Created on: 3 Dec 2015
 *      Author: alanb
 */

#include "PMTriggerRun.h"

PMTriggerRun::PMTriggerRun() :
	_trigger(0)
{
	_task_window.name("PackMan install/remove trigger");
	_task_window.wimp_slot(256);
	_task_window.options(tbx::TaskWindow::ALLOW_CONTROL|tbx::TaskWindow::QUIT);
	_task_window.add_output_listener(this);
	_task_window.add_finished_listener(this);
}

PMTriggerRun::~PMTriggerRun()
{
}

/**
 * Start the given trigger
 *
 * @param file_name to run
 * @param trigger trigger to inform of status
 */
void PMTriggerRun::run(const std::string &file_name, pkg::trigger *trigger)
{
	std::string name;
	name += trigger->pkgname();
	name += " - PkgTrigger";
	_task_window.name(name);
	_task_window.command(file_name);
	try
	{
		_trigger = trigger;
		_task_window.run();
	} catch(std::exception &e)
	{
		trigger->trigger_start_failed(e.what());
		_trigger = 0;
	}
}

/**
 * Trigger has finished call back
 */
void PMTriggerRun::taskwindow_finished(tbx::TaskWindow &/*task_window*/)
{
	if (_trigger)
	{
		_trigger->trigger_finished();
		_trigger = 0;
	}
}

/**
 * Output from trigger window
 */
void PMTriggerRun::taskwindow_output(tbx::TaskWindow &/*task_window*/, int size, const char *text)
{
	if (_trigger)
	{
		std::string msg(text, size);
		_trigger->trigger_log(msg);
	}
}
