/*********************************************************************
* This file is part of PackMan
*
* Copyright 2015-2020 AlanBuckley
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
