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
 * CopyrightWindow.cc
 *
 *  Created on: 19-Aug-2009
 *      Author: alanb
 */

#include "CopyrightWindow.h"
#include "MainWindow.h"
#include "Packages.h"

#include "tbx/window.h"
#include "tbx/deleteonhidden.h"

#include "libpkg/pkgbase.h"


CopyrightWindow::CopyrightWindow(tbx::Object obj) : _view(obj, true)
{
	tbx::Window window(obj);

	window.add_about_to_be_shown_listener(this);
}

CopyrightWindow::~CopyrightWindow()
{
}

/**
 * Find currently selected package and show copyright
 */
void CopyrightWindow::about_to_be_shown(tbx::AboutToBeShownEvent &event)
{
	MainWindow *main = MainWindow::from_window(event.id_block().ancestor_object());
	const pkg::binary_control *ctrl = main->selected_package();
	if (ctrl != 0)
	{
		std::string pkgname = ctrl->pkgname();

		tbx::Window window(event.id_block().self_object());
		window.title("Copyright for " + pkgname);

		// Open manifest.
		pkg::pkgbase *pb = Packages::instance()->package_base();
		std::string pathname=pb->info_pathname(pkgname)+std::string(".Copyright");
		_view.load_file(pathname);
	}
}
