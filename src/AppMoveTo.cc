/*********************************************************************
* This file is part of PackMan
*
* Copyright 2012-2020 AlanBuckley
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
 * AppMoveTo.cc
 *
 *  Created on: 8 May 2012
 *      Author: alanb
 */

#include "AppMoveTo.h"
#include "AppsWindow.h"
#include "MoveWindow.h"
#include "MoveExistsWindow.h"
#include "PackManState.h"

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
}

/**
 * Do actual move
 */
void AppMoveTo::saveas_save_to_file(tbx::SaveAs saveas, bool selection, std::string filename)
{
	if (_source_path.canonical_equals(filename))
	{
		tbx::show_message("Source and destination for the move must be different");
	} else if (pmstate()->ok_to_move())
	{
		tbx::Path dst(filename);
		if (dst.leaf_name() != _source_path.leaf_name())
		{
		   tbx::show_message("Destination leaf name must be the same as the application");
		} else if (dst.exists())
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

