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
 * InstallCommand.cc
 *
 *  Created on: 25-Mar-2009
 *      Author: alanb
 */

#include "Commands.h"
#include "MainWindow.h"
#include "IRWindow.h"
#include "CommitWindow.h"
#include "UpdateListWindow.h"
#include "SourcesWindow.h"
#include "PathsWindow.h"
#include "BackupWindow.h"
#include "Packages.h"
#include "PackManState.h"
#include "UpgradeAllWindow.h"
#include "VerifyWindow.h"

#include "tbx/deleteonhidden.h"
#include "tbx/questionwindow.h"

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
		if (_install == 0) _install = new IRWindow(false);
		_install->set_package(pkg_control);
		_install->show();
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
		if (_remove == 0) _remove = new IRWindow(true);
		_remove->set_package(pkg_control);
		_remove->show();
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
	if (!UpdateListWindow::showing() && pmstate()->installed())
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
			if (_upgrade == 0) _upgrade = new UpgradeAllWindow();
			if (_upgrade->set_upgrades())
			{
			   _upgrade->show();
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
	std::string q("This option will verify that the files exist\n"
			"for all your installed packages.\n"
			"It does not check that the contents are correct.\n\n"
			"If you install/upgrade/remove or move a package during\n"
			"the process it may give a false result.\n\n"
			"Do you wish to run the verify now?"
			);
	tbx::show_question_as_menu(q, "Verify All", &_verify_all_command);
}
