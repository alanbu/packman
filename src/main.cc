/*********************************************************************
* Copyright 2009 Alan Buckley
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
#include "CopyrightWindow.h"
#include "SearchWindow.h"
#include "InstallWindow.h"

// Have to set filename converstion or download.cc
// in libpkg has a linkage problem
#include "unixlib/local.h"
int __riscosify_control = __RISCOSIFY_NO_PROCESS;

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
 * Entry point for program
 */
int main(int argc, char *argv[])
{
	tbx::Application packman("<PackMan$Dir>");
	tbx::Iconbar iconbar("IconbarIcon");

	// Attach auto created objects to C++ classes to handle them
	packman.set_autocreate_listener("MainMenu", new tbx::AutoCreateClass<MainMenu>() );
	packman.set_autocreate_listener("PackageMenu", new tbx::AutoCreateClass<PackageMenu>() );
	packman.set_autocreate_listener("Info", new tbx::AutoCreateClass<InfoWindow>() );

	// Windows that probably won't be used as often so create on demand
	tbx::MatchLifetime<AppsWindow> mlt_apps("Apps");
	tbx::MatchLifetime<AppsMenu> mlt_apps_menu("AppsMenu");
	tbx::MatchLifetime<AppSaveAs> mlt_app_save_as("AppSaveAs");
	tbx::MatchLifetime<CopyrightWindow> mlt_copyright("Copyright");
	tbx::MatchLifetime<SearchWindow> mlt_search("Search");

	iconbar.add_click_command(new ShowMainWindowCommand());
	iconbar.add_loader(new PackageLoader());
	iconbar.add_command(UpdateListCommand::COMMAND_ID, new UpdateListCommand());
	iconbar.add_command(ShowSourcesWindowCommand::COMMAND_ID, new ShowSourcesWindowCommand());

	iconbar.show();
	packman.run();

	return 0;
}
