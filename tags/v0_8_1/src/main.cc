/*********************************************************************
* Copyright 2009-2013 Alan Buckley
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
 * main.cc
 *
 *  Created on: 18-Mar-2009
 *      Author: alanb
 */

#include "tbx/application.h"
#include "tbx/iconbar.h"
#include "tbx/command.h"
#include "tbx/reporterror.h"
#include "tbx/autocreate.h"
#include "tbx/matchlifetime.h"
#include "tbx/showhelp.h"
#include "tbx/uncaughthandler.h"
#include "tbx/taskmanager.h"

#include "Packages.h"
#include "MainWindow.h"
#include "PackageLoader.h"
#include "Commands.h"
#include "MainMenu.h"
#include "PackageMenu.h"
#include "InfoWindow.h"
#include "AppsWindow.h"
#include "AppsMenu.h"
#include "AppSaveAs.h"
#include "AppMoveTo.h"
#include "CopyrightWindow.h"
#include "SearchWindow.h"
#include "InstallWindow.h"
#include "CacheWindow.h"
#include "BootOptionsWindow.h"

#include <fstream>
#include <ctime>
#include "stdlib.h"


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
			main->set_filter("Installed");
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
	    			char *ver = getenv("PackMan$Version");


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
 * Check if PackMan is already running and displays a message if it is.
 *
 * @returns true if PackMan is already running
 */
bool already_running()
{
	char *running_var = getenv("PackMan$Running");
	if (running_var)
	{
		// Safety check in case last stopped because of a crash
		tbx::TaskManager tm;
		if (tm.running("Package Manager"))
		{
			tbx::report_error("PackMan is already running - You can only have one copy running at a time!");
			return true;
		}
	}
	return false;
}


/**
 * Entry point for program
 */
int main(int argc, char *argv[])
{
	if (already_running()) return 0;
	setenv("PackMan$Running","Yes",1);

	tbx::Application packman("<PackMan$Dir>");
	tbx::Iconbar iconbar("IconbarIcon");
	tbx::ShowHelp show_help;

	// Attach auto created objects to C++ classes to handle them
	packman.set_autocreate_listener("MainMenu", new tbx::AutoCreateClass<MainMenu>() );
	packman.set_autocreate_listener("PackageMenu", new tbx::AutoCreateClass<PackageMenu>() );
	packman.set_autocreate_listener("Cache", new tbx::AutoCreateClassOnce<CacheWindow>());

	// Commands that can be run from anywhere
	packman.add_command(HELP_COMMAND_ID, &show_help);
	packman.add_command(UpgradeAllCommand::COMMAND_ID, new UpgradeAllCommand());
	packman.add_command(UpdateListCommand::COMMAND_ID, new UpdateListCommand());
	packman.add_command(ShowLogViewerCommand::COMMAND_ID, new ShowLogViewerCommand());

	// Windows that probably won't be used as often so create on demand
	tbx::MatchLifetime<AppsWindow> mlt_apps("Apps");
	tbx::MatchLifetime<AppsMenu> mlt_apps_menu("AppsMenu");
	tbx::MatchLifetime<AppSaveAs> mlt_app_save_as("AppSaveAs");
	tbx::MatchLifetime<AppMoveTo> mlt_app_move_to("AppMoveTo");
	tbx::MatchLifetime<CopyrightWindow> mlt_copyright("Copyright");
	tbx::MatchLifetime<SearchWindow> mlt_search("Search");
	tbx::MatchLifetime<BootOptionsWindow> mlt_boot_options("BootOpts");

	iconbar.add_select_command(new ShowMainWindowCommand());
	iconbar.add_adjust_command(new ShowInstalledCommand());
	iconbar.add_loader(new PackageLoader());
	iconbar.menu().add_command(ShowSourcesWindowCommand::COMMAND_ID, new ShowSourcesWindowCommand());
	iconbar.menu().add_command(ShowPathsWindowCommand::COMMAND_ID, new ShowPathsWindowCommand());
	iconbar.menu().add_command(ShowBackupWindowCommand::COMMAND_ID, new ShowBackupWindowCommand());
	iconbar.menu().add_command(VerifyAllAskCommand::COMMAND_ID, new VerifyAllAskCommand());

	ReportUncaught error_handler;
	packman.uncaught_handler(&error_handler);

	iconbar.show();
	packman.run();

	unsetenv("PackMan$Running");

	return 0;
}
