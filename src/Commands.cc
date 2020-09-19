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
 * InstallCommand.cc
 *
 *  Created on: 25-Mar-2009
 *      Author: alanb
 */

#include "Commands.h"
#include "MainWindow.h"
#include "CommitWindow.h"
#include "UpdateListWindow.h"
#include "SourcesWindow.h"
#include "PathsWindow.h"
#include "BackupWindow.h"
#include "Packages.h"
#include "PackManState.h"
#include "VerifyWindow.h"
#include "LogViewer.h"
#include "PackageConfigWindow.h"
#include "InfoWindow.h"
#include "ChoicesWindow.h"
#include "CopyrightWindow.h"
#include "FindWindow.h"
#include "InstallListSaveAs.h"
#include "EnvWindow.h"

#include "tbx/deleteonhidden.h"
#include "tbx/questionwindow.h"
#include "tbx/messagewindow.h"

#include <string>

/**
 * Run the install command
 */
void InstallCommand::execute()
{
	const pkg::binary_control *pkg_control = _main->selected_package();
	if (pkg_control == 0) return;

	if (pmstate()->ok_to_commit())
	{
		Packages::instance()->select_install(pkg_control);
		PackageConfigWindow::update();
	}
}

/**
 * Run the remove command
 */
void RemoveCommand::execute()
{
	const pkg::binary_control *pkg_control = _main->selected_package();
	if (pkg_control == 0) return;

	if (pmstate()->ok_to_commit())
	{
		Packages::instance()->select_remove(pkg_control);
		PackageConfigWindow::update();
	}
}

/**
 * Install or remove selected packages
 */
void CommitCommand::execute()
{
	if (!CommitWindow::showing())
	{
		new CommitWindow();
	}
}

/**
 * Update list of available packages by downloading
 * lastest lists from the servers.
 */
void UpdateListCommand::execute()
{
	if (!UpdateListWindow::running() && pmstate()->installed())
	{
		new UpdateListWindow();
	}
}

/**
 * Show window to allow updating of sources
 */
void ShowSourcesWindowCommand::execute()
{
	if (pmstate()->installed())
	{
		new SourcesWindow();
	}
}

/**
 * Show window to display currently set paths
 */
void ShowPathsWindowCommand::execute()
{
	if (pmstate()->installed())
	{
		new PathsWindow();
	}
}

/**
 * Run the Upgrade all command
 */
void UpgradeAllCommand::execute()
{
	if (pmstate()->installed())
	{
		if (pmstate()->ok_to_commit())
		{
			bool upgrades = Packages::instance()->select_upgrades();
			if (upgrades)
			{
				PackageConfigWindow::update();
				PackageConfigWindow::bring_to_top();
			} else
			{
			   tbx::Window no_upgrades("NoUpgrades");
			   no_upgrades.add_has_been_hidden_listener(new tbx::DeleteObjectOnHidden());
			}
		}
	}
}

/**
 * Show window to maintain backups made for the user by PackMan
 */
void ShowBackupWindowCommand::execute()
{
	if (pmstate()->installed())
	{
		BackupWindow::show();
	}
}

/**
 * Verify all installed packages.
 */
void VerifyAllCommand::execute()
{
	new VerifyWindow();
}

/**
 * Display message to check it its OK to run the verify all process
 * and run it if it is.
 */
void VerifyAllAskCommand::execute()
{
	if (pmstate()->installed())
	{
		std::string q("This option will verify that the files exist\n"
				"for all your installed packages.\n"
				"It does not check that the contents are correct.\n\n"
				"If you install/upgrade/remove or move a package during\n"
				"the process it may give a false result.\n\n"
				"Do you wish to run the verify now?"
				);
		tbx::show_question_as_menu(q, "Verify All", &_verify_all_command);
	}
}


/**
 * Show Whats New in filter in a new window
 */
void ShowWhatsNewCommand::execute()
{
	if (pmstate()->installed())
	{
		MainWindow *main = new MainWindow();
		main->set_status_filter("What's New");
	}
}

/**
 * Local class to turn on logging - just used by show log viewer command for now
 */
 class TurnOnLoggingCommand : public tbx::Command
 {
    public:
       void execute()
       {
          Packages::instance()->logging(true);
       }
};

/**
 * Show a log viewer if possible
 */
 void ShowLogViewerCommand::execute()
 {
     Packages *p = Packages::instance();
     std::tr1::shared_ptr<pkg::log> lg = p->current_log();
     if (lg)
     {
         new LogViewer(lg);
     } else if (p->logging())
     {
         tbx::show_message("No logs have been created yet");
     } else
     {
         tbx::show_question_as_menu(
           "No logs have been created yet and no logs"
           " will be produced as logging is turned off.\n\n"
           "Do you wish to turn logging on now?",
           "Show Log Viewer",
           new TurnOnLoggingCommand(), 0, true);
     }
 }

 /**
  * Show the information window for the selected package
  */
void ShowInfoCommand::execute()
{
	const pkg::binary_control *pkg_control = _main->selected_package();
	if (pkg_control == 0) return;

	InfoWindow::show(pkg_control);
}

/**
 * Command to show the choices window
 */
void ShowChoicesWindowCommand::execute()
{
	if (pmstate()->installed())
	{
		ChoicesWindow::show();
	}
}

/**
 * Show the copyright window
 */
void CopyrightCommand::execute()
{
	const pkg::binary_control *pkg_control = _main->selected_package();
	if (pkg_control == 0) return;

	new CopyrightWindow(pkg_control);
}

/**
 * Show the find file window
 */
void FindFileCommand::execute()
{
	if (pmstate()->installed())
	{
		new FindWindow();
	}
}

void ShowEnvironmentCommand::execute()
{
	EnvWindow::show();
}
