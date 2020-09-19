/*********************************************************************
* This file is part of PackMan
*
* Copyright 2015-2020 AlanBuckley
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
