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
/*
 * RecommendsWindow.h
 *
 *  Created on: 26 Sep 2013
 *      Author: alanb
 */

#ifndef RECOMMENDSWINDOW_H_
#define RECOMMENDSWINDOW_H_

#include <string>
#include <vector>

#include "tbx/window.h"
#include "tbx/command.h"
#include "tbx/res/reswindow.h"
#include "tbx/res/resoptionbutton.h"


/**
 * Class to show recommendations and suggestions in a window
 * and allow them to be added to the packages to be installed
 */
class RecommendsWindow
{
	std::vector<std::string> _packages;
	tbx::Window _window;
	tbx::CommandMethod<RecommendsWindow> _add_command;
public:
	RecommendsWindow(std::vector<std::string> &recommends, std::vector<std::string> &suggests);
	virtual ~RecommendsWindow();

	void add();

private:
	void add_option(std::string pkgname, tbx::res::ResOptionButton &res_option, int &button_move_y);

};

#endif /* RECOMMENDSWINDOW_H_ */
