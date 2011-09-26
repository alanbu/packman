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
 * UpgradeAllWindow.h
 *
 *  Created on: 25-Mar-2009
 *      Author: alanb
 */

#ifndef UPGRADEALLWINDOW_H_
#define UPGRADEALLWINDOW_H_

#include "libpkg/binary_control.h"
#include "tbx/window.h"
#include "tbx/displayfield.h"
#include "tbx/scrolllist.h"
#include "tbx/actionbutton.h"

/**
 * Class to show dialog to confirm upgrading of all packages */
class UpgradeAllWindow
{
	tbx::Window _window;;

	// Fields
	tbx::ScrollList _upgrades;
	tbx::DisplayField _upgrades_number;
	tbx::DisplayField _upgrades_size;
	tbx::ScrollList _auto_remove;
	tbx::DisplayField _remove_number;
	tbx::ActionButton _install_button;

public:
	UpgradeAllWindow();
	virtual ~UpgradeAllWindow();

	bool set_upgrades();
	void show();

private:
	unsigned int download_size(std::string pkgname);
};

#endif /* UPGRADEALLWINDOW_H_ */
