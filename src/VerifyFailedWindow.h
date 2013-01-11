/*********************************************************************
* Copyright 2013 Alan Buckley
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

#ifndef VERFIYFAILEDWINDOW_H_
#define VERFIYFAILEDWINDOW_H_

#include "Verify.h"

#include "tbx/window.h"
#include "tbx/command.h"
#include <set>
#include <vector>

/**
 * Class to give a graphical way to handle conflicts
 * reported by the commit process
 */
class VerifyFailedWindow
{
	tbx::Window _window;
	tbx::CommandMethod<VerifyFailedWindow> _show_files;
	tbx::CommandMethod<VerifyFailedWindow> _how_to_fix;

	std::vector<std::string> _missing_files;

public:
	VerifyFailedWindow(const std::vector<Verify::FailedPackage> &failures);
	virtual ~VerifyFailedWindow();

	void show_files();
	void how_to_fix();
};

#endif /* VERFIYFAILEDWINDOW_H_ */
