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
 * InstallWindow.cc
 *
 *  Created on: 21-Aug-2009
 *      Author: alanb
 */

#include "InstallWindow.h"

#include "libpkg/filesystem.h"
#include "tbx/hourglass.h"

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
	system(cmd.c_str());

	_saveas.file_save_completed(true, filename);
}

