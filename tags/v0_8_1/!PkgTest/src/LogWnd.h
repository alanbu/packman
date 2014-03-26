/*********************************************************************
* Copyright 2014 Alan Buckley
*
* This file is part of PkgTest (PackMan unit testing)
*
* PackMan is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* PkgTest is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with PackMan. If not, see <http://www.gnu.org/licenses/>.
*
*****************************************************************************/
/*
 * LogWnd.h
 *
 *  Created on: 14 Feb 2014
 *      Author: alanb
 */

#ifndef LOGWND_H_
#define LOGWND_H_

#include "tbx/view/listview.h"
#include "tbx/view/itemrenderer.h"
#include "tbx/view/viewitems.h"
#include "tbx/deleteonhidden.h"

#include <string>
#include <vector>

class LogWnd : private tbx::DeleteObjectOnHidden
{
	std::vector<std::string> _lines;
	tbx::view::IndexItemViewValue<std::string,std::vector<std::string> > _view_value;
	tbx::view::WimpFontItemRenderer _renderer;
	tbx::view::ListView _view;
public:
	LogWnd(const std::string &log_path);
	virtual ~LogWnd();
};

#endif /* LOGWND_H_ */
