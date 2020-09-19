/*********************************************************************
* This file is part of PackMan
*
* Copyright 2009-2020 AlanBuckley
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
 * AppSaveAs.cc
 *
 *  Created on: 29-Jul-2009
 *      Author: alanb
 */

#include "AppSaveAs.h"
#include "AppsWindow.h"
#include "CreateStub.h"

#include "tbx/objectdelete.h"
#include "tbx/hourglass.h"
#include "tbx/messagewindow.h"
#include "tbx/fileraction.h"
#include "tbx/stringutils.h"

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
	case COPY: _saveas.title("Copy"); break;
	}

	// Menu item id specifies type of save
	_source_path = apps_window->selected_app_path();
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
		create_application_stub(_source_path, filename);
		break;

	case LINK: // Create a link
		create_application_link(_source_path, filename);
		break;
	}

	_saveas.file_save_completed(true, filename);
}
