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
 * AppSaveAs.h
 *
 *  Created on: 29-Jul-2009
 *      Author: alanb
 */

#ifndef APPSAVEAS_H_
#define APPSAVEAS_H_

#include "tbx/saveas.h"
#include "tbx/abouttobeshownlistener.h"
#include "tbx/path.h"

/**
 * Class to handle creating links or copying
 * and application.
 */
class AppSaveAs :
	tbx::AboutToBeShownListener,
	tbx::SaveAsSaveToFileHandler
{
	tbx::SaveAs _saveas;
	tbx::Path _source_path;
	enum SaveType {STUB, LINK, COPY} _save_type;

	virtual void about_to_be_shown(tbx::AboutToBeShownEvent &event);
	virtual void saveas_save_to_file(tbx::SaveAs saveas, bool selection, std::string filename);

public:
	AppSaveAs(tbx::Object obj);
	virtual ~AppSaveAs();


};

#endif /* APPSAVEAS_H_ */
