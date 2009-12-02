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
 * StringListView.h
 *
 *  Created on: 28-Jul-2009
 *      Author: alanb
 */

#ifndef STRINGLISTVIEW_H_
#define STRINGLISTVIEW_H_

#include "ListView.h"
#include <vector>
#include <string>

/**
 * List view class that handles a vector of strings
 */
class StringListView : public ListView
{
	std::vector<std::string> _lines;

	// Class to render a line
	class LineRenderer : public WimpFontItemRenderer
	{
		StringListView *_view;
	public:
		LineRenderer(StringListView *view) : _view(view) {};
		virtual ~LineRenderer() {};

		virtual std::string text(unsigned int index) const {return _view->line(index);}
	} _line_renderer;


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
    	unsigned int old_count = row_count();
    	_lines.assign(first, last);
    	unsigned int new_count = _lines.size();
    	if (old_count < new_count)
    	{
    		changed(0, old_count);
    		inserted(old_count, new_count-old_count);
    	}
    	else if (old_count > new_count)
    	{
    		changed(0, new_count);
    		removed(new_count, old_count-new_count);
    	}
    	else changed(0, new_count);
    }
};

#endif /* STRINGLISTVIEW_H_ */
