/*********************************************************************
* This file is part of PackMan
*
* Copyright 2013-2020 AlanBuckley
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
