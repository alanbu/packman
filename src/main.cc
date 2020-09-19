/*********************************************************************
* This file is part of PackMan
*
* Copyright 2009-2020 AlanBuckley
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
#include "FileLoader.h"
#include "Commands.h"
#include "MainMenu.h"
#include "PackageMenu.h"
#include "InfoWindow.h"
#include "AppsWindow.h"
#include "AppsMenu.h"
#include "AppSaveAs.h"
#include "AppMoveTo.h"
#include "SearchWindow.h"
#include "CacheWindow.h"
#include "BootOptionsWindow.h"
#include "Choices.h"
#include "MainCommands.h"
#include "InstallListSaveAs.h"
#include "PackManState.h"

#include "stdlib.h"

// Functions defined at end of file
bool already_running();
void prompt_for_update_lists();

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
	tbx::MatchLifetime<SearchWindow> mlt_search("Search");
	tbx::MatchLifetime<BootOptionsWindow> mlt_boot_options("BootOpts");

	iconbar.add_select_command(new ShowMainWindowCommand());
	iconbar.add_adjust_command(new ShowInstalledCommand());
	iconbar.add_loader(new FileLoader());
	iconbar.menu().add_command(ShowChoicesWindowCommand::COMMAND_ID, new ShowChoicesWindowCommand());
	iconbar.menu().add_command(ShowSourcesWindowCommand::COMMAND_ID, new ShowSourcesWindowCommand());
	iconbar.menu().add_command(ShowPathsWindowCommand::COMMAND_ID, new ShowPathsWindowCommand());
	iconbar.menu().add_command(ShowBackupWindowCommand::COMMAND_ID, new ShowBackupWindowCommand());
	iconbar.menu().add_command(VerifyAllAskCommand::COMMAND_ID, new VerifyAllAskCommand());
	iconbar.menu().add_command(FindFileCommand::COMMAND_ID, new FindFileCommand());
	iconbar.menu().add_command(ShowEnvironmentCommand::COMMAND_ID, new ShowEnvironmentCommand());

	// Windows that are auto created by toolbox and reached by a menu
	tbx::MatchLifetime<InstallListSaveAs> mlt_install_list_save_as("InstList");

	ReportUncaught error_handler;
	packman.uncaught_handler(&error_handler);

	choices().load();

	if (pmstate()->installed(&iconbar))
	{
		Packages::instance()->logging(choices().enable_logging());
		prompt_for_update_lists();
		iconbar.show();
	}

	packman.run();

	unsetenv("PackMan$Running");

	return 0;
}


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
 * Check if we should prompt to update the lists
 */
void prompt_for_update_lists()
{
	int prompt_days = choices().update_prompt_days();
	if (prompt_days < 0) return; // Never prompt
	if (getenv("Packages$Dir") == 0) return; // Not installed
	tbx::Path master_list("<Packages$Dir>.Available");
	tbx::UTCTime last_changed = master_list.modified_time();
	if (last_changed.centiseconds() == 0) return; // File doesn't exists
    int last_day =  (int)(last_changed.centiseconds() / 8640000LL);
    int this_day = (int)(tbx::UTCTime::now().centiseconds() / 8640000LL);
    int days_since_update = this_day - last_day;

   if (days_since_update > prompt_days)
   {
	   tbx::app()->add_idle_command(new PromptForUpdateListsCommand(days_since_update));
   }
}
