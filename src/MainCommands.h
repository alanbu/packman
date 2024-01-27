/*********************************************************************
* This file is part of PackMan
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
 * MainCommands.h
 *
 *  Created on: 5 Aug 2014
 *      Author: alanb
 *
 *  Commands used by main.cc
 */

#ifndef MAINCOMMANDS_H_
#define MAINCOMMANDS_H_

#include "tbx/application.h"
#include "tbx/questionwindow.h"
#include "tbx/stringutils.h"

#include <fstream>
#include <ctime>
#include <cstdlib>

/**
 * Show the main packman window if it's not already shown
 */
class ShowMainWindowCommand : public tbx::Command
{
public:
	virtual void execute()
	{
		new MainWindow();
	}
};

/**
 * Show the main window with the "Installed" filter selected
 */
class ShowInstalledCommand : public tbx::Command
{
public:
	virtual void execute()
	{
		MainWindow *main = new MainWindow();
		main->set_status_filter("Installed");
	}
};

/**
 * Class to add some logging to uncaught exceptions
 */
class ReportUncaught : public tbx::UncaughtHandler
{
public:
	virtual void uncaught_exception(std::exception *e, int event_code)
	{
	    try
	    {
	    	std::string msg;
	    	msg = "An unexpected error has occurred. ";
	    	msg+= "PackMan will attempt to continue, but it would be ";
	    	msg+= "advisable to close PackMan and restart it.";
	    	msg+= " The error was: ";
	    	msg+= ((e) ? e->what() : "An unknown exception");
	    	if (msg.size() > 200)
	    	{
	    		msg.erase(200);
	    		msg += "..";
	    	}

	    	msg+= ". Please report it to the PackMan author.";

	    	try
	    	{
	    		std::ofstream log("<PackMan$Dir>.errorlog", std::ofstream::app);
	    		if (log)
	    		{
	    			std::time_t rawtime;
	    			struct std::tm * timeinfo;
	    			char *ver = std::getenv("PackMan$Version");


	    			std::time (&rawtime);
	    			timeinfo = std::localtime (&rawtime);

	    			log << "Unexpected exception in PackMan";
	    			if (ver) log << " version " << ver << std::endl;
	    			log << " on:    " << std::asctime(timeinfo) << std::endl;
					log << " event code: " << event_code << std::endl;
	    			log << " error: " << ((e) ? e->what() : "An unknown exception") << std::endl;
	    			log << std::endl;
	    			log.close();
	    		}
	    	} catch(...)
	    	{
	    		// Ignore log write errors
	    	}

	    	tbx::report_error(msg);
	    } catch(...)
	    {
	    	tbx::report_error("Exception in uncaught handler - please report to PackMan author");
	    }
	}
};

/**
 * Command to prompt to see if the user would like to update the
 * package lists
 */
class PromptForUpdateListsCommand : public tbx::Command
{
	int _days_since_update;
public:
	PromptForUpdateListsCommand(int days_since_update) : _days_since_update(days_since_update)
	{
	}

	void execute()
	{
		tbx::app()->remove_idle_command(this);
		std::string msg;
		if (_days_since_update == 1)
		{
			msg = "The list of packages has not been updated today.";
		} else
		{
			msg  = "The list of packages has not been updated for ";
			msg += tbx::to_string(_days_since_update);
			msg += " days.";
		}
		msg +="\n\nWould you like to update the list from the Internet now?";

		tbx::show_question(msg,
				"PackMan",
				new UpdateListCommand(),
				0,
				true);

		delete this;
	}
};




#endif /* MAINCOMMANDS_H_ */
