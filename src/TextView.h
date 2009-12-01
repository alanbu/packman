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
 * TextView.h
 *
 *  Created on: 31-Jul-2009
 *      Author: alanb
 */

#ifndef TEXTVIEW_H_
#define TEXTVIEW_H_

#include "tbx/window.h"
#include "tbx/redrawlistener.h"
#include "tbx/margin.h"
#include <vector>

/**
 * Class to display text in a window
 */
class TextView :
	public tbx::RedrawListener
{
private:
	tbx::Window _window;
	tbx::Margin _margin;
	bool _wrap;
	char *_text;
	unsigned int _size;
	std::vector<unsigned int> _line_end;
	unsigned int _width;

public:
	TextView(tbx::Window window, bool wrap = false);
	virtual ~TextView();

	void update_window_extent();
	void refresh();

	const tbx::Margin &margin() const {return _margin;}
	void margin(const tbx::Margin &margin);

	const char *text() const {return _text;}
	void text(const char *text);
	void text(const std::string &text);

    unsigned int size() const {return _size;}

    bool load_file(const std::string &file_name);

    bool wrap() const {return _wrap;}
    void wrap(bool w);

	// Redraw listener override
	virtual void redraw(const tbx::RedrawEvent &event);

private:
	void recalc_layout();
};

#endif /* TEXTVIEW_H_ */
