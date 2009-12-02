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
 * TextView.cc
 *
 *  Created on: 31-Jul-2009
 *      Author: alanb
 */

#include "TextView.h"
#include <cstring>
#include "tbx/font.h"
#include <fstream>

using namespace tbx;

const char ROW_HEIGHT = 32;

/**
 * Construct a text view.
 *
 * @param window to show it in
 * @param true to wrap to window width. Default false
 */
TextView::TextView(tbx::Window window, bool wrap /*= false*/) :
	_window(window),
	_wrap(wrap),
	_text(0),
	_size(0),
	_width(0)
{
	_window.add_redraw_listener(this);
}

TextView::~TextView()
{
	delete [] _text;
}

/**
 * Set the text for the text view
 */
void TextView::text(const char *text)
{
	_size = std::strlen(text);
	delete [] _text;
	if (_size)
	{
		_text = new char[_size+1];
		std::strcpy(_text, text);
	}
	else _text = 0;
	recalc_layout();
}

/**
 * Set the text for the text view
 */
void TextView::text(const std::string &text)
{
	_size = text.size();
	delete [] _text;
	if (_size)
	{
		_text = new char[_size+1];
		text.copy(_text, _size);
		_text[_size] = 0;
	}
	else _text = 0;
	recalc_layout();
}

/**
 * Set wrap to width on or off
 */
void TextView::wrap(bool w)
{
	if (w != _wrap)
	{
		_wrap = w;
		recalc_layout();
	}
}

/**
 * Change the margin
 */
void TextView::margin(const tbx::Margin &margin)
{
	_margin = margin;
	update_window_extent();
	refresh();
}

/**
 * Update the Window extent after a change in size.
 *
 * @param visible_area visible area of window on screen
 * @param repaint true to refresh the contents
 */
void TextView::update_window_extent()
{
	int width = _width + _margin.left + _margin.right;
	int height = _line_end.size() * ROW_HEIGHT + _margin.top + _margin.bottom;

	WindowState state;
	_window.get_state(state);
	if (width < state.visible_area().bounds().width())
		width = state.visible_area().bounds().width();

	if (height < state.visible_area().bounds().height())
	   height = state.visible_area().bounds().height();

	BBox extent(0,-height, width, 0);
	_window.extent(extent);
}

/**
 * Refresh the whole report view.
 *
 * It should also be called if all the data changes.
 */

void TextView::refresh()
{
	BBox all(_margin.left, -_margin.top - _line_end.size() * ROW_HEIGHT,
			_margin.left + _width, -_margin.top);
	_window.force_redraw(all);
}

/**
 * Redraw the window
 */
void TextView::redraw(const RedrawEvent &event)
{
	if (_text == 0 || _size == 0) return; // Nothing to draw

	BBox work_clip = event.visible_area().work(event.clip());

	unsigned int first_row = (-work_clip.max.y - _margin.top) / ROW_HEIGHT;
	unsigned int last_row =  (-work_clip.min.y - _margin.top) / ROW_HEIGHT;

	printf("first row %d last row %d\n", first_row, last_row);

	if (first_row < 0) first_row = 0;
	if (last_row < 0) return; // Nothing to draw

	if (first_row >= _line_end.size()) return; // Nothing to draw
	if (last_row >= _line_end.size()) last_row = _line_end.size() - 1;

	tbx::WimpFont font;
	font.set_font_colours(tbx::Colour::black, tbx::Colour::white);

	int x = event.visible_area().screen_x(_margin.left);
	int y = event.visible_area().screen_y(-_margin.top - (first_row+1) * ROW_HEIGHT) + 4;
	int start = (first_row == 0) ? 0 : _line_end[first_row-1];

	for (unsigned int row = first_row; row <= last_row; row++)
	{
		int end = _line_end[row];
		if (_text[start] == '\n') start++;
		if (start < end)
		{
			font.paint(x, y, _text + start, end-start);
		}
		start = end;
		y -= ROW_HEIGHT;
	}
}

/**
 * Recalculate the text layout after text is changed or
 * window resized.
 */
void TextView::recalc_layout()
{
	_line_end.clear();
	tbx::Font font; // Can't use WimpFont as it doesn't have split function
	font.desktop_font();

	if (_wrap)
	{
		// Wrap to window
		int pos = 0;
		int start;
		WindowState state;
		_window.get_state(state);
		_width = state.visible_area().bounds().width();

		while (pos < (int)_size)
		{
			start = pos;
			pos = font.find_split_os(_text + start, _size - start, _width, '\n') + start;
			_line_end.push_back(pos);
			if (_text[pos] == '\n') pos++;
		}
	} else
	{
		char *p = _text;
		char *start;
		char *end = p + _size;
		unsigned int line_width;
		_width = 0;

		while (p != end)
		{
			start = p;
			p = std::strchr(start, '\n');
			if (p == 0) p = end;
			_line_end.push_back(p - start);
			line_width = font.string_width_os(start, p - start);
			if (line_width > _width) _width = line_width;
			if (p != end) p++;
		}
	}

	update_window_extent();
	refresh();
}

/**
 * Load text for view from file
 */
bool TextView::load_file(const std::string &file_name)
{
	std::ifstream is(file_name.c_str());
	bool loaded = false;
	_size = 0;
	delete [] _text;
	_text = 0;

	if (is)
	{
		is.seekg (0, std::ios::end);
		_size = is.tellg();
		is.seekg (0, std::ios::beg);

		_text = new char[_size+1];
		is.read (_text,_size);
		_text[_size] = 0;
		if (!is.fail()) loaded = true;
		is.close();
	}

	recalc_layout();

	return loaded;
}
