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
 * PathChooser.h
 *
 *  Created on: 24 Apr 2013
 *      Author: alanb
 */

#ifndef PATHCHOOSER_H_
#define PATHCHOOSER_H_

#include "tbx/draggable.h"
#include "tbx/button.h"
#include "tbx/saver.h"
#include "tbx/loader.h"

class PackageConfigWindow;

/**
 * Helper class to update the path name gadget on the PathConfigWnd
 * by dragging a draggable or dropping a path onto the path name
 * gadget.
 */
class PathChooser :
		public tbx::DragEndedListener,
		private tbx::SaverSaveToFileHandler,
		private tbx::Loader
{
	PackageConfigWindow *_config_wnd;
	tbx::Button _saver_update_field;

public:
	PathChooser(PackageConfigWindow *config_wnd);
	virtual ~PathChooser();

	void add_path_listeners(int idx);

	virtual void drag_ended(const tbx::DragEndedEvent &drag_ended_event);

private:
	virtual void saver_save_to_file(tbx::Saver saver, std::string file_name);

	// Loader interface overrides
	virtual bool load_file(tbx::LoadEvent &event);
	virtual bool accept_file(tbx::LoadEvent &event);


};

#endif /* PATHCHOOSER_H_ */
