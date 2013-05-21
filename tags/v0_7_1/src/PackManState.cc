/*********************************************************************
* Copyright 2012 Alan Buckley
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
 * PackManState.cc
 *
 *  Created on: 14 May 2012
 *      Author: alanb
 */

#include "PackManState.h"
#include "Packages.h"
#include "InstallWindow.h"
#include "CommitWindow.h"
#include "tbx/messagewindow.h"

/**
 * Global PackManState class
 */
PackManState g_pmstate;
PackManState *PackManState::_instance = 0;

PackManState::PackManState() :
	_moving(false)
{
	_instance = this;
}

PackManState::~PackManState()
{
	_instance = 0;
}

/**
 * Check if the !Packages files is installed and prompt
 * to install it if it isn't
 *
 * If not installed it prompts for the install and returns
 * false.
 *
 * @returns true if !Packages installed.
 *
 */
bool PackManState::installed()
{
	if (Packages::instance()->ensure_package_base())
	{
		return true;
	} else
	{
		new InstallWindow();
		return false;
	}
}

/**
 * Check if commit window is available.
 *
 * @return true if an operation that requires the commit window can be run
 */
bool PackManState::ok_to_commit()
{
	bool ok = false;
	if (CommitWindow::running())
	{
		show_busy(COMMIT, COMMIT);
	} else if (_moving)
	{
		show_busy(COMMIT, MOVE);
	} else
	{
		ok = true;
	}

	return ok;
}


/**
 * Set if a move operation is in action
 *
 * @param m true if a move is running, false otherwise
 */
void PackManState::moving(bool m)
{
	_moving = m;
}

/**
 * Check if it's OK to start a move operation
 *
 * @returns true if it's OK to start a move
 */
bool PackManState::ok_to_move()
{
	bool ok = false;
	if (_moving)
	{
		show_busy(MOVE, MOVE);
	} else if (CommitWindow::running())
	{
		show_busy(MOVE, COMMIT);
	} else
	{
		ok = true;
	}

	return ok;
}

/**
 * Show message is packman is busy
 *
 * @param trying state we were seeing if we could put it in
 * @param why_not state that stopped it
 */
void PackManState::show_busy(StateType trying, StateType why_not)
{
	std::string msg("Unable to start ");
	msg += state_text(trying);
	msg += "\nas ";
	msg += "there is ";
	if (trying == why_not) msg += "already ";
	msg += state_text(why_not) + " running.";
	msg += "\n\nPlease try again when it finishes.";
	tbx::show_message(msg, "PackMan", "information");
}

/**
 * Return a string describing a type
 */
std::string PackManState::state_text(StateType type)
{
	std::string text;

	switch(type)
	{
	case COMMIT: text = "an install/remove or upgrade"; break;
	case MOVE:   text = "a move of an application or path"; break;
	case NONE:   text = "an action"; break;
	}

	return text;
}
