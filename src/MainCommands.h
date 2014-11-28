/*********************************************************************
* Copyright 2014 Alan Buckley
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
		if (Packages::instance()->ensure_package_base())
		{
			new MainWindow();
		} else
		{
			new InstallWindow();
		}
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
		if (Packages::instance()->ensure_package_base())
		{
			MainWindow *main = new MainWindow();
			main->set_status_filter("Installed");
		} else
		{
			new InstallWindow();
		}
	}
};

/**
 * Class to add some logging to uncaught exceptions
 */
class ReportUncaught : public tbx::UncaughtHandler
{
public:
	virtual void uncaught_exception(std::exception *e)
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
	    			log << " on:    " << std::asctime(timeinfo);
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
