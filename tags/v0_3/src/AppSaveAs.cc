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
 * AppSaveAs.cc
 *
 *  Created on: 29-Jul-2009
 *      Author: alanb
 */

#include "AppSaveAs.h"
#include "AppsWindow.h"

#include "tbx/objectdelete.h"
#include "tbx/hourglass.h"

#include <fstream>

/**
 * Set up listeners to set up and do the save
 */
AppSaveAs::AppSaveAs(tbx::Object obj) : _saveas(obj)
{
	_saveas.add_about_to_be_shown_listener(this);
	_saveas.set_save_to_file_handler(this);
}

AppSaveAs::~AppSaveAs()
{
}

/**
 * Setup dialog for show
 */
void AppSaveAs::about_to_be_shown(tbx::AboutToBeShownEvent &event)
{
	AppsWindow *apps_window = AppsWindow::from_window(event.id_block().ancestor_object());
	int id = event.id_block().parent_component().id();
	_save_type = SaveType(id % 3);

	switch(_save_type)
	{
	case COPY: _saveas.title("Copy application"); break;
	case STUB: _saveas.title("Create Stub"); break;
	case LINK: _saveas.title("Create link"); break;
	}

	// Menu item id specifies type of save
	_source_path = apps_window->selected_app_path();
	_saveas.filename(_source_path.leafname());
}

/** Create obey file to link from stub to target.
 * Take no action if the target file does not exist.
 * @param path the location to create the link
 * @param leaf the item to link
 */
void AppSaveAs::link(const tbx::Path &path, std::string leaf)
{
	tbx::Path src(_source_path, leaf);
	tbx::Path dst(path, leaf);
	if (src.exists())
	{
		std::string contents("/");
		contents += src.name();
		contents += " %*0\n";
		dst.save_file(contents.c_str(), contents.size(), 0xfeb);
	}
}


/**
 * Do actual save
 */
void AppSaveAs::saveas_save_to_file(tbx::SaveAs saveas, bool selection, std::string filename)
{
	tbx::Hourglass hg;

	switch(_save_type)
	{
	case COPY: // Copy file
		_source_path.copy(filename, tbx::Path::COPY_RECURSE);
		break;

	case STUB: // Create an application stub
		{
			tbx::Path dst(filename);
			if (dst.create_directory())
			{
				link(dst, "!Boot");
				link(dst, "!Run");
				link(dst, "!Help");
			}
		}
		break;

	case LINK: // Create a link
		{
			std::ofstream out(filename.c_str());
			out << _source_path.name() << '.' << char(0);
			out.close();
			tbx::Path dst(filename);
			dst.file_type(0xfc0);
		}
		break;
	}
	_saveas.file_save_completed(true, filename);
}