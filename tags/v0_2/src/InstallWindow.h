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
 *  Created on: 21-Aug-2009
 *      Author: alanb
 */

#ifndef INSTALLWINDOW_H_
#define INSTALLWINDOW_H_

#include "tbx/saveas.h"
#include "tbx/abouttobeshownlistener.h"

/**
 * Class to set up !Packages
 */
class InstallWindow :
	tbx::AboutToBeShownListener,
	tbx::SaveAsSaveToFileHandler
{
private:
	tbx::SaveAs _saveas;
public:
	InstallWindow();
	virtual ~InstallWindow();

	virtual void about_to_be_shown(tbx::AboutToBeShownEvent &event);
	virtual void saveas_save_to_file(tbx::SaveAs saveas, bool selection, std::string filename);
};

#endif /* INSTALLWINDOW_H_ */
