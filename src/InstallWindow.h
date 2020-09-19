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
 * InstallWindow.h
 *
 *  Created on: 21-Aug-2009
 *      Author: alanb
 */

#ifndef INSTALLWINDOW_H_
#define INSTALLWINDOW_H_

#include "tbx/saveas.h"
#include "tbx/abouttobeshownlistener.h"
#include "tbx/iconbar.h"

/**
 * Class to set up !Packages
 */
class InstallWindow :
	tbx::AboutToBeShownListener,
	tbx::SaveAsSaveToFileHandler,
	tbx::SaveAsDialogueCompletedListener
{
private:
	tbx::SaveAs _saveas;
	tbx::Iconbar *_iconbar;
	bool _installed;
public:
	InstallWindow(tbx::Iconbar *iconbar = nullptr);
	virtual ~InstallWindow();

	virtual void about_to_be_shown(tbx::AboutToBeShownEvent &event);
	virtual void saveas_save_to_file(tbx::SaveAs saveas, bool selection, std::string filename);
	virtual void saveas_dialogue_completed(const tbx::SaveAsDialogueCompletedEvent &completed_event);
};

#endif /* INSTALLWINDOW_H_ */
