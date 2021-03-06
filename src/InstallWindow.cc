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
	tbx::Iconbar *_iconbar;
public:
	InstallDbSuccess(tbx::Iconbar *iconbar) : _iconbar(iconbar) {}
	virtual void execute();
};

InstallWindow::InstallWindow(tbx::Iconbar *iconbar /*= iconbar */) :
		_saveas("InstPackApp"),
		_iconbar(iconbar),
		_installed(false)
{
	_saveas.add_about_to_be_shown_listener(this);
	_saveas.set_save_to_file_handler(this);
	_saveas.add_dialogue_completed_listener(this);

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
	_installed = true;
	std::string cmd=std::string("Filer_Run ")+filename;
	try
	{
	    tbx::app()->os_cli(cmd);
	    tbx::app()->add_idle_command(new InstallDbSuccess(_iconbar));
	} catch(...)
	{
	   // Shouldn't get here
	}

	_saveas.file_save_completed(true, filename);
}

/**
 * Dialogue has been completed so quit if package db was not setup
 */
void InstallWindow::saveas_dialogue_completed(const tbx::SaveAsDialogueCompletedEvent &completed_event)
{

	if (!_installed)
	{
		tbx::app()->quit();
	}
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
		if (_iconbar) _iconbar->show();
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
		tbx::app()->quit();
	}

	delete this;
}
