/*********************************************************************
* Copyright 2013 Alan Buckley
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
 * PathChooser.cpp
 *
 *  Created on: 24 Apr 2013
 *      Author: alanb
 */

#include "PathChooser.h"
#include "PackageConfigWindow.h"
#include "tbx/path.h"

/**
 * Constructor to set the field to be updated
 *
 * The class will then need to be added to a draggable using add_drag_ended_listener
 *
 * @param leaf_name name of file to be dragged
 * @param update_field field to update
 */
PathChooser::PathChooser(PackageConfigWindow *config_wnd) :
  _config_wnd(config_wnd)
{

}

/**
 * Add listeners for the a given path entry
 *
 * Uses PathConfigWnd to get actual ids
 *
 * @param idx - 0 based index of component entry to add listeners for
 */
void PathChooser::add_path_listeners(int idx)
{
	tbx::Draggable draggable = _config_wnd->draggable_gadget(idx);
	tbx::Button update_field = _config_wnd->path_gadget(idx);

	draggable.add_drag_ended_listener(this);
	update_field.add_loader(this);
}

PathChooser::~PathChooser()
{
}

/**
 * Check where item is and start save to get path name
 *
 * @param drag_ended_event details of draggable drag ended
 */
void PathChooser::drag_ended(const tbx::DragEndedEvent &drag_ended_event)
{
	tbx::Draggable draggable = drag_ended_event.id_block().self_component();
	_saver_update_field = _config_wnd->path_gadget(draggable);
	std::string leaf_name = _config_wnd->leaf_gadget(draggable).text();
	tbx::Saver saver;
	saver.set_save_to_file_handler(this);
	saver.save(drag_ended_event.where(), leaf_name, 0xFFF, 0 );
}

/**
 * Save to file completed
 */
void PathChooser::saver_save_to_file(tbx::Saver saver, std::string file_name)
{
    if (saver.safe())
    {
    	tbx::Path path(file_name);
    	_saver_update_field.value(path.parent());
    	_saver_update_field.foreground(tbx::WimpColour::black);
    }

	// Never do an actual save so mark it as failed.
	saver.file_save_completed(false, file_name);
}


/**
 * Copy file name to the button
 *
 * @param event details of file
 * @returns false as we will never really load the file
 */
bool PathChooser::load_file(tbx::LoadEvent &event)
{
	tbx::Button update_field = event.destination_gadget();
	update_field.value(event.file_name());
	update_field.foreground(tbx::WimpColour::black);
	return false;
}

/**
 * Allow dropping of applications or directories
 *
 * @param load_event details of what has been dropped
 * @returns true if application or directory
 */
bool PathChooser::accept_file(tbx::LoadEvent &event)
{
	return (event.file_type() == tbx::FILE_TYPE_DIRECTORY || event.file_type() == tbx::FILE_TYPE_APPLICATION);
}
