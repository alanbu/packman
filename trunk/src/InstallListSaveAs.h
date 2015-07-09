/*********************************************************************
* Copyright 2015 Alan Buckley
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
 * InstallListSaveAs.h
 *
 *  Created on: 7 May 2015
 *      Author: alanb
 */

#ifndef INSTALLLISTSAVEAS_H_
#define INSTALLLISTSAVEAS_H_

#include "tbx/saveas.h"

/**
 * Class to handle saving the install list
 */
class InstallListSaveAs :
	tbx::SaveAsSaveToFileHandler
{
public:
	InstallListSaveAs(tbx::Object obj);
	virtual ~InstallListSaveAs();

	virtual void saveas_save_to_file(tbx::SaveAs saveas, bool selection, std::string filename);
};

#endif /* INSTALLLISTSAVEAS_H_ */
