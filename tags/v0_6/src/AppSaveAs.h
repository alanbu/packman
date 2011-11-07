/*********************************************************************
* Copyright 2009 Alan Buckley
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
	enum SaveType {COPY, STUB, LINK} _save_type;

	virtual void about_to_be_shown(tbx::AboutToBeShownEvent &event);
	virtual void saveas_save_to_file(tbx::SaveAs saveas, bool selection, std::string filename);

public:
	AppSaveAs(tbx::Object obj);
	virtual ~AppSaveAs();


};

#endif /* APPSAVEAS_H_ */
