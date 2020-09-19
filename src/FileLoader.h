/*********************************************************************
* This file is part of PackMan
*
* Copyright 2009-2015 AlanBuckley
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
 * FileLoader.h
 *
 *  Created on: 29 Jun 2015
 *      Author: alanb
 */

#ifndef FILELOADER_H_
#define FILELOADER_H_

#include <tbx/loader.h>

/**
 * Class to look at file dropped on icon and redirect it
 * to the code to load it depending on it's contents
 */
class FileLoader: public tbx::Loader
{
public:
	FileLoader();
	virtual ~FileLoader();

	virtual bool load_file(tbx::LoadEvent &event);
};

#endif /* FILELOADER_H_ */
