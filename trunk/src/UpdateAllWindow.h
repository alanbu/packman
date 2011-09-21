/*********************************************************************
* Copyright 2011 Alan Buckley
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
 * UpdateAllWindow.h
 *
 *  Created on: 25-Mar-2009
 *      Author: alanb
 */

#ifndef UPDATEALLWINDOW_H_
#define UPDATEALLWINDOW_H_

#include "libpkg/binary_control.h"
#include "tbx/window.h"
#include "tbx/displayfield.h"
#include "tbx/scrolllist.h"
#include "tbx/actionbutton.h"

/**
 * Class to show dialog to confirm updating of all packages */
class UpdateAllWindow
{
	tbx::Window _window;;

	// Fields
	tbx::ScrollList _updates;
	tbx::DisplayField _updates_number;
	tbx::DisplayField _updates_size;
	tbx::ScrollList _auto_remove;
	tbx::DisplayField _remove_number;
	tbx::ActionButton _install_button;

public:
	UpdateAllWindow();
	virtual ~UpdateAllWindow();

	bool set_updates();
	void show();

private:
	unsigned int download_size(std::string pkgname);
};

#endif /* UPDATEALLWINDOW_H_ */
