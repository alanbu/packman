/*********************************************************************
* Copyright 2009-2015 Alan Buckley
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
 * CopyrightWindow.h
 *
 *  Created on: 19-Aug-2009
 *      Author: alanb
 */

#ifndef COPYRIGHTWINDOW_H_
#define COPYRIGHTWINDOW_H_

#include "tbx/view/textview.h"
#include "libpkg/binary_control.h"

/**
 * Class to show copyright of currently selected package
 */
class CopyrightWindow
{
	tbx::view::TextView _view;

public:
	CopyrightWindow(const pkg::binary_control *ctrl);
	virtual ~CopyrightWindow();
};

#endif /* COPYRIGHTWINDOW_H_ */
