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
 * AppMoveTo.cc
 *
 *  Created on: 8 May 2012
 *      Author: alanb
 */

#include "AppMoveTo.h"
#include "AppsWindow.h"
#include "MoveWindow.h"
#include "MoveExistsWindow.h"

#include "tbx/hourglass.h"
#include "tbx/messagewindow.h"
#include "tbx/stringutils.h"


AppMoveTo::AppMoveTo(tbx::Object obj) : _saveas(obj)
{
	_saveas.add_about_to_be_shown_listener(this);
	_saveas.set_save_to_file_handler(this);
}

AppMoveTo::~AppMoveTo()
{

}

/**
 * Setup dialog for show
 */
void AppMoveTo::about_to_be_shown(tbx::AboutToBeShownEvent &event)
{
	AppsWindow *apps_window = AppsWindow::from_window(event.id_block().ancestor_object());

	_source_path = apps_window->selected_app_path();
	_logical_path = apps_window->selected_app_logical_path();
	_saveas.file_name(_source_path.leaf_name());

	std::string title = _saveas.title();
	std::string::size_type rep_pos = title.find('@');

	if (rep_pos != std::string::npos)
	{
		title.replace(rep_pos, 1, _source_path.leaf_name());
		_saveas.title(title);
	}
}

/**
 * Do actual move
 */
void AppMoveTo::saveas_save_to_file(tbx::SaveAs saveas, bool selection, std::string filename)
{
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

	_saveas.file_save_completed(true, filename);
}

