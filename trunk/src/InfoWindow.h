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
 * InfoWindow.h
 *
 *  Created on: 24-Jul-2009
 *      Author: alanb
 */

#ifndef INFOWINDOW_H_
#define INFOWINDOW_H_

#include "tbx/window.h"
#include "tbx/displayfield.h"
#include "tbx/textarea.h"
#include "tbx/abouttobeshownlistener.h"

/**
 * Class to handle window showing information on the
 * selected package
 */
class InfoWindow : tbx::AboutToBeShownListener
{
	tbx::Window _window;
	tbx::DisplayField _installed;
	tbx::TextArea _description;
public:
	InfoWindow(tbx::Object object);
	virtual ~InfoWindow();

	void about_to_be_shown(tbx::AboutToBeShownEvent &event);
};

#endif /* INFOWINDOW_H_ */
