/*********************************************************************
* Copyright 2009-2010 Alan Buckley
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
#include "CreateStub.h"
#include "MoveWindow.h"
#include "MoveExistsWindow.h"

#include "tbx/objectdelete.h"
#include "tbx/hourglass.h"
#include "tbx/messagewindow.h"
#include "tbx/fileraction.h"
#include "tbx/stringutils.h"

#include <fstream>
#include <stdexcept>

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
	_save_type = SaveType(id - 1);

	switch(_save_type)
	{
	case STUB: _saveas.title("Create Stub"); break;
	case LINK: _saveas.title("Create link"); break;
	case COPY: _saveas.title("Copy application"); break;
	case MOVE: _saveas.title("Move application"); break;
	}

	// Menu item id specifies type of save
	_source_path = apps_window->selected_app_path();
	_logical_path = apps_window->selected_app_logical_path();
	_saveas.file_name(_source_path.leaf_name());
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
		try
		{
			tbx::Path target(filename);
			if (target.leaf_name() != _source_path.leaf_name())
			{
				// Non multi-tasking copy need if leaf name changes
				_source_path.copy(filename, tbx::Path::COPY_RECURSE);
			} else
			{
				// Use FilerAction to get a multi-tasking copy
				tbx::FilerAction fa(_source_path);
				fa.copy(target.parent(), tbx::FilerAction::VERBOSE);
			}
		} catch(std::runtime_error &e)
		{
			std::string msg("Unable to start filer copy\n");
			msg += e.what();
			tbx::show_message(msg, "PackMan", "warning");
		}
		break;

	case STUB: // Create an application stub
		{
			tbx::Path dst(filename);
			create_application_stub(_source_path, dst);
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

	case MOVE:
		if (_source_path.canonical_equals(filename))
		{
			tbx::show_message("Source and destination for the move must be different");
		} else
		{
			tbx::Path dst(filename);

			if (dst.exists())
			{
				// Create and show a window to prompt for overwrite then
				// do the move.
				new MoveExistsWindow(_logical_path, _source_path, filename);
			} else
			{
				// Check destination isn't a subdirectory of the original
				tbx::Path src_check(_source_path), dst_check(dst);
				src_check.canonicalise();
				dst_check.canonicalise();
				std::string::size_type src_len = src_check.name().size();

				if (dst_check.name().size() > src_len
					&& dst_check.name()[src_len] == '.'
					&& tbx::equals_ignore_case(dst_check.name().substr(0, src_len), src_check.name())
					)
				{
					tbx::show_message("Destination of a move can not be a subdirectory of the source of the move");
				} else
				{
					// Do the move directly
					new MoveWindow(_logical_path, _source_path, filename);
				}
			}
		}
		break;
	}
	_saveas.file_save_completed(true, filename);
}
