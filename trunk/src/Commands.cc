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
#include "tbx/deleteonhidden.h"

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
