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
 * StringListView.h
 *
 *  Created on: 28-Jul-2009
 *      Author: alanb
 */

#ifndef STRINGLISTVIEW_H_
#define STRINGLISTVIEW_H_

#include "tbx/view/listview.h"
#include "tbx/view/itemrenderer.h"

#include <vector>
#include <string>

/**
 * List view class that handles a vector of strings
 */
class StringListView
{
	std::vector<std::string> _lines;

	tbx::view::IndexItemViewValue< std::string, std::vector<std::string> > _item_value;
	tbx::view::WimpFontItemRenderer _line_renderer;
	tbx::view::ListView _view;

public:
	StringListView(tbx::Window window);
	virtual ~StringListView();

	void clear();
	void push_back(std::string value);
	void pop_back();

	std::string line(int index) {return _lines[index];}
	void line(int index, std::string new_value);

	/**
	 * Assign lines from another collection
	 */
    template<typename InputIterator> void assign(InputIterator first, InputIterator last)
    {
    	unsigned int old_count = _view.count();
    	_lines.assign(first, last);
    	unsigned int new_count = _lines.size();
    	if (old_count < new_count)
    	{
    		_view.changed(0, old_count);
    		_view.inserted(old_count, new_count-old_count);
    	}
    	else if (old_count > new_count)
    	{
    		_view.changed(0, new_count);
    		_view.removed(new_count, old_count-new_count);
    	}
    	else _view.changed(0, new_count);
    }
};

#endif /* STRINGLISTVIEW_H_ */
