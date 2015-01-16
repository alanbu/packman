/*********************************************************************
* Copyright 2009-2014 Alan Buckley
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
 * InstallWindow.cc
 *
 *  Created on: 21-Aug-2009
 *      Author: alanb
 */

#include "InstallWindow.h"
#include "Packages.h"
#include "Commands.h"

#include "libpkg/filesystem.h"
#include "libpkg/pkgbase.h"
#include "libpkg/path_table.h"
#include "tbx/hourglass.h"
#include "tbx/application.h"
#include "tbx/messagewindow.h"
#include "tbx/questionwindow.h"
#include <cstdlib>

/**
 * Local class for package database successful install
 * extra processing
 */
class InstallDbSuccess : public tbx::Command
{
public:
	virtual void execute();
};

InstallWindow::InstallWindow() : _saveas("InstPackApp")
{
	_saveas.add_about_to_be_shown_listener(this);
	_saveas.set_save_to_file_handler(this);

//	_saveas.add_object_deleted_listener(new tbx::ObjectDeleteClass<AppSaveAs>(this));

}

InstallWindow::~InstallWindow()
{
}

/**
 * Setup dialog for show
 */
void InstallWindow::about_to_be_shown(tbx::AboutToBeShownEvent &event)
{
	std::string default_path=pkg::canonicalise("<Boot$Dir>.Resources.!Packages");

	_saveas.file_name(default_path);
}

/**
 * Do actual copy
 */
void InstallWindow::saveas_save_to_file(tbx::SaveAs saveas, bool selection, std::string filename)
{
	tbx::Hourglass hg;

	pkg::copy_object("<PackMan$Dir>.Resources.!Packages", filename);
	std::string cmd=std::string("Filer_Run ")+filename;
	try
	{
	    tbx::app()->os_cli(cmd);
	    tbx::app()->add_idle_command(new InstallDbSuccess());
	} catch(...)
	{
	   // Shouldn't get here
	}

	_saveas.file_save_completed(true, filename);
}

/**
 * Package database has been successfully installed
 * so read it back in and update PackMan path
 * and prompt to do an update lists
 */
void InstallDbSuccess::execute()
{
	tbx::app()->remove_idle_command(this);
	Packages *p = Packages::instance();
	if (p->ensure_package_base())
	{
		pkg::pkgbase *pkg_base = p->package_base();
		std::string packman_path = pkg::canonicalise("<PackMan$Dir>");
		if (pkg_base->paths()("Apps.Admin.!PackMan","") != packman_path)
		{
			packman_path = pkg::boot_drive_relative(packman_path);
			pkg_base->paths().alter("Apps.Admin.!PackMan", packman_path);
			pkg_base->paths().commit();
		}
		tbx::show_question("!Packages has been successfully installed\n"
				"You now need to update the list of packages available\n"
				"from the Internet.\n\n"
				"Do you wish to update the list now?",
				"PackMan",
				new UpdateListCommand(),
				0,
				true);
	} else
	{
		tbx::show_message("Failed to create !Packages","PackMan","error");
	}

	delete this;
}
