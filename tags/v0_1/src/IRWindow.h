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
 * InstallWindow.h
 *
 *  Created on: 25-Mar-2009
 *      Author: alanb
 */

#ifndef INSTALLWINDOW_H_
#define INSTALLWINDOW_H_

#include "libpkg/binary_control.h"
#include "tbx/window.h"
#include "tbx/displayfield.h"
#include "tbx/scrolllist.h"
#include "tbx/actionbutton.h"

/**
 * Class to show dialog to confirm install/remove
 */
class IRWindow
{
	tbx::Window _window;
	bool _remove;

	// Fields
	tbx::DisplayField _package;
	tbx::DisplayField _version;
	tbx::DisplayField _summary;
	tbx::ScrollList _install_depends;
	tbx::DisplayField _install_number;
	tbx::DisplayField _install_size;
	tbx::ScrollList _auto_remove;
	tbx::DisplayField _remove_number;
	tbx::ActionButton _install_button;

public:
	IRWindow(bool install);
	virtual ~IRWindow();

	void set_package(const pkg::binary_control *bctrl);
	void show();

private:
	unsigned int download_size(std::string pkgname);
};

#endif /* INSTALLWINDOW_H_ */
