/*********************************************************************
* This file is part of PackMan
*
* Copyright 2013-2020 AlanBuckley
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
