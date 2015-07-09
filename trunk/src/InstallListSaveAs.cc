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
 * InstallListSaveAs.cc
 *
 *  Created on: 7 May 2015
 *      Author: alanb
 */

#include "InstallListSaveAs.h"
#include "InstallList.h"
#include "tbx/hourglass.h"

/**
 * Construct from save as object
 */
InstallListSaveAs::InstallListSaveAs(tbx::Object obj)
{
	tbx::SaveAs saveas(obj);
	saveas.set_save_to_file_handler(this);
}

InstallListSaveAs::~InstallListSaveAs()
{
}

/**
 * Save install list to given file
 */
void InstallListSaveAs::saveas_save_to_file(tbx::SaveAs saveas, bool selection, std::string filename)
{
	//TODO: read sources/paths from dialog
	InstallList install_list(true, true);
	tbx::Hourglass hg(true);
	install_list.save(filename);
}
