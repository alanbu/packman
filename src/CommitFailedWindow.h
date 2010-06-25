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
 * CommitFailedWindow.h
 *
 *  Created on: 19-Aug-2009
 *      Author: alanb
 */

#ifndef COMMITFAILEDWINDOW_H_
#define COMMITFAILEDWINDOW_H_

#include <string>

namespace pkg
{
   class commit;
}

/**
 * Class to show failure message from install/remove
 */
class CommitFailedWindow
{
public:
	CommitFailedWindow(pkg::commit *commit, std::string last_action);
	virtual ~CommitFailedWindow();
};

#endif /* COMMITFAILEDWINDOW_H_ */
