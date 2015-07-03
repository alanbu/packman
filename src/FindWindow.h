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
 * FindWindow.h
 *
 *  Created on: 13 May 2015
 *      Author: alanb
 */

#ifndef FINDWINDOW_H_
#define FINDWINDOW_H_

#include "tbx/window.h"
#include "tbx/command.h"
#include "tbx/loader.h"
#include "tbx/writablefield.h"

/**
 * Window used to prompt to find a file
 */
class FindWindow
{
	tbx::Window _window;
	tbx::CommandMethod<FindWindow> _find_command;

	/**
	 * Loaded to put filename in given icon
	 */
	class DraggedFileName : public tbx::Loader
	{
		tbx::WritableField _load_to;

	public:
		DraggedFileName(tbx::Gadget writable) : _load_to(writable)
		{
			_load_to.add_loader(this);
		}

		/**
		 * Copy file name to writable field
		 */
		virtual bool load_file(tbx::LoadEvent &event)
		{
		   if (event.from_filer())
		   {
			   _load_to.text(event.file_name());
			   return true;
		   }
		   return false;
		}
	} _filename_loader;

public:
	FindWindow();
	FindWindow(const std::string &find_text);
	virtual ~FindWindow();

	void find();

private:
	void init();
};

#endif /* FINDWINDOW_H_ */
