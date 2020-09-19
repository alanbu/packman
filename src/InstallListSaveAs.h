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
 * InstallListSaveAs.h
 *
 *  Created on: 7 May 2015
 *      Author: alanb
 */

#ifndef INSTALLLISTSAVEAS_H_
#define INSTALLLISTSAVEAS_H_

#include "tbx/saveas.h"

/**
 * Class to handle saving the install list
 */
class InstallListSaveAs :
	tbx::SaveAsSaveToFileHandler
{
public:
	InstallListSaveAs(tbx::Object object);
	virtual ~InstallListSaveAs();

	virtual void saveas_save_to_file(tbx::SaveAs saveas, bool selection, std::string filename);
};

#endif /* INSTALLLISTSAVEAS_H_ */
