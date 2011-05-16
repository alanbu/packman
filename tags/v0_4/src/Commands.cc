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
#include "InstallWindow.h"
#include "UpdateListWindow.h"
#include "SourcesWindow.h"
#include "Packages.h"
#include "tbx/deleteonhidden.h"

/**
 * Run the install command
 */
void InstallCommand::execute()
{
	const pkg::binary_control *pkg_control = _main->selected_package();
	if (pkg_control == 0) return;

	if (CommitWindow::showing())
	{
		CommitWindow *cw = CommitWindow::instance();
		// If it's showing at the end of a completed operation
		// we can close it.
		if (cw->done()) cw->close();
		else
		{
			tbx::Window only_one("OnlyOne");
			only_one.add_has_been_hidden_listener(new tbx::DeleteObjectOnHidden());
			return;
		}
	}

	if (_install == 0) _install = new IRWindow(false);
	_install->set_package(pkg_control);
	_install->show();
}

/**
 * Run the remove command
 */
void RemoveCommand::execute()
{
	const pkg::binary_control *pkg_control = _main->selected_package();
	if (pkg_control == 0) return;

	if (CommitWindow::showing())
	{
		CommitWindow *cw = CommitWindow::instance();
		// If it's showing at the end of a completed operation
		// we can close it.
		if (cw->done()) cw->close();
		else
		{
			tbx::Window only_one("OnlyOne");
			only_one.add_has_been_hidden_listener(new tbx::DeleteObjectOnHidden());
			return;
		}
	}

	if (_remove == 0) _remove = new IRWindow(true);
	_remove->set_package(pkg_control);
	_remove->show();
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
	if (!UpdateListWindow::showing())
	{
		if (Packages::instance()->ensure_package_base())
		{
			new UpdateListWindow();
		} else
		{
			new InstallWindow();
		}
	}
}

/**
 * Show window to allow updating of sources
 */
void ShowSourcesWindowCommand::execute()
{
	if (Packages::instance()->ensure_package_base())
	{
		new SourcesWindow();
	} else
	{
		new InstallWindow();
	}
}
