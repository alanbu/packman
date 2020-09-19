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
 * InstallListSaveAs.cc
 *
 *  Created on: 7 May 2015
 *      Author: alanb
 */

#include "InstallListSaveAs.h"
#include "InstallList.h"
#include "Packages.h"
#include "tbx/hourglass.h"
#include "tbx/deleteonhidden.h"
#include "tbx/messagewindow.h"

/**
 * Construct from save as object
 */
InstallListSaveAs::InstallListSaveAs(tbx::Object object)
{
	tbx::SaveAs saveas(object);
	saveas.set_save_to_file_handler(this);
}

InstallListSaveAs::~InstallListSaveAs()
{
}

/**
 * Save install list to given file
 */
void InstallListSaveAs::saveas_save_to_file(tbx::SaveAs saveas, bool selection, std::string filename)
{
	//TODO: read sources/paths from dialog
	InstallList install_list(true, true);
	tbx::Hourglass hg(true);
	install_list.save(filename);
	saveas.file_save_completed(true, filename);
}
