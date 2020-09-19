/*********************************************************************
* This file is part of PkgTest (PackMan unit testing)
*
* Copyright 2014-2020 AlanBuckley
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
 * LogWnd.h
 *
 *  Created on: 14 Feb 2014
 *      Author: alanb
 */

#ifndef LOGWND_H_
#define LOGWND_H_

#include "tbx/view/listview.h"
#include "tbx/view/itemrenderer.h"
#include "tbx/view/viewitems.h"
#include "tbx/deleteonhidden.h"

#include <string>
#include <vector>

class LogWnd : private tbx::DeleteObjectOnHidden
{
	std::vector<std::string> _lines;
	tbx::view::IndexItemViewValue<std::string,std::vector<std::string> > _view_value;
	tbx::view::WimpFontItemRenderer _renderer;
	tbx::view::ListView _view;
public:
	LogWnd(const std::string &log_path);
	virtual ~LogWnd();
};

#endif /* LOGWND_H_ */
