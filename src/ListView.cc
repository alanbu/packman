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
 * ListView.cc
 *
 *  Created on: 24-Jul-2009
 *      Author: alanb
 */

#include "ListView.h"

#include "tbx/font.h"
#include "tbx/graphics.h"

#include <stdexcept>

using namespace tbx;

/**
 * Construct a report view for a window.
 *
 * Adds listeners it needs to the windows
 */
ListView::ListView(Window window, ItemRenderer *item_renderer /*= 0*/) :
	_window(window), _rows(0),
    _height(32), _width(0),
    _selection(0),
    _item_renderer(item_renderer)
{
	_window.add_redraw_listener(this);
}

/**
 * Destructor of report view
 *
 * removes any window listeners it used
 */
ListView::~ListView()
{
	_window.remove_redraw_listener(this);
	if (_selection)
	{
		_selection->remove_listener(this);
		_window.remove_mouse_click_listener(this);
	}
}

/**
 * Change the row height
 */
void ListView::row_height(unsigned int height)
{
	if (height != _height)
	{
		_height = height;
		if (_rows)
		{
			update_window_extent();
			refresh();
		}
	}
}


/**
 * Set the selection model to use for the list view.
 *
 * @param selection Selection model to use for selecting items in the list
 */
void ListView::selection(tbx::Selection *selection)
{
	if (selection == _selection) return;

	if (_selection)
	{
		_selection->remove_listener(this);
		delete _selection;
		if (selection == 0) _window.remove_mouse_click_listener(this);
	} else if (selection != 0)
	{
		_window.add_mouse_click_listener(this);
	}

	_selection = selection;
	if (_selection) _selection->add_listener(this);
}

/**
 * Update the Window extent after a change in size.
 *
 * @param visible_area visible area of window on screen
 * @param repaint true to refresh the contents
 */
void ListView::update_window_extent()
{
	int width = _width + _margin.left + _margin.right;
	int height = _rows * _height + _margin.top + _margin.bottom;

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

void ListView::refresh()
{
	BBox all(_margin.left, -_margin.top - _rows * _height,
			_margin.left + _width, -_margin.top);
	_window.force_redraw(all);
}

/**
 * Change the margin
 */
void ListView::margin(const tbx::Margin &margin)
{
	_margin = margin;
	update_window_extent();
	refresh();
}

/**
 * Change the renderer for the list view
 *
 * @param cr Renderer to render cells for the column
 */
void ListView::item_renderer(ItemRenderer *ir)
{
	_item_renderer = ir;
	refresh();
}

/**
 * Set the width
 */
void ListView::width(unsigned int width)
{
	if (width != _width)
	{
		_width = width;
		if (_rows)
		{
			update_window_extent();
		}
	}
}

/**
 * Size width to match content.
 * Does nothing if there is no content
 */
void ListView::autosize(unsigned int min_width/* = 0*/, unsigned int max_width /*= 0xFFFFFFFF*/)
{
	if (_item_renderer == 0 || _rows == 0) return;
	unsigned int new_width = min_width;

	for (unsigned int row = 0; row < _rows; row++)
	{
		unsigned int size = _item_renderer->width(row);
		if (size > max_width) size = max_width;
		if (size > new_width) new_width = size;
	}

	if (new_width != _width)
		width(new_width);
}

/**
 * Call after inserting rows into the collection
 * the ListView is showing.
 *
 */
void ListView::inserted(unsigned int where, unsigned int how_many)
{
	//TODO: Use window block copy to update
	_rows += how_many;
	update_window_extent();

	int first_row = where / _height;
	int last_row = row_count() / _height + 1;

	if (_selection) _selection->inserted(where, how_many);

	BBox dirty(_margin.left,
		-last_row * _height - _margin.top,
		_width + _margin.left,
		-first_row * _height - _margin.top);
	_window.force_redraw(dirty);
}

/**
 * Call after removing rows from the collection
 * the ListView is showing.
 */
void ListView::removed(unsigned int where, unsigned int how_many)
{
	//TODO: Use window block copy to update
	int first_row = where / _height;
	int last_row = row_count() / _height + 1;
	BBox dirty(_margin.left,
		-last_row * _height - _margin.top,
		_width + _margin.left,
		-first_row * _height - _margin.top);
	_rows -= how_many;
	if (_selection) _selection->removed(where, how_many);
	update_window_extent();
	_window.force_redraw(dirty);
}

/**
 * Call after changing rows in the collection
 * the ListView is showing.
 */
void ListView::changed(unsigned int where, unsigned int how_many)
{
	int first_row = where / _height;
	int last_row = _rows / _height+ 1;
	BBox dirty(_margin.left,
		-last_row * _height - _margin.top,
		_width + _margin.left,
		-first_row * _height - _margin.top);
	_window.force_redraw(dirty);
}

/**
 * Whole view has been cleared
 */
void ListView::cleared()
{
	if (_rows)
	{
		refresh();
		_rows = 0;
		if (_selection) _selection->clear();
		update_window_extent();
		WindowState state;
		_window.get_state(state);
		Point &scroll = state.visible_area().scroll();
		if (scroll.x != 0 || scroll.y != 0)
		{
			scroll.x = 0;
			scroll.y = 0;
			_window.open_window(state);
		}
	}
}

/**
 * Redraw the window
 */
void ListView::redraw(const RedrawEvent &event)
{
	BBox work_clip = event.visible_area().work(event.clip());

	unsigned int first_row = (-work_clip.max.y - _margin.top) / _height;
	unsigned int last_row =  (-work_clip.min.y - _margin.top) / _height;

	if (first_row < 0) first_row = 0;
	if (last_row < 0) return; // Nothing to draw

	if (first_row >= _rows) return; // Nothing to draw
	if (last_row >= _rows) last_row = _rows - 1;

    ItemRenderer::Info cell_info(event);

	cell_info.bounds.max.y = -first_row * _height - _margin.top;
	cell_info.screen.y = event.visible_area().screen_y(cell_info.bounds.max.y);

	cell_info.screen.x = event.visible_area().screen_x(_margin.left);
	cell_info.bounds.min.x = _margin.left;
    cell_info.bounds.max.x = _margin.left + _width;

    for (unsigned int row = first_row; row <= last_row; row++)
    {
        cell_info.bounds.min.y = cell_info.bounds.max.y - _height;
        cell_info.screen.y -= _height;
        cell_info.index = row;

        cell_info.selected = (_selection != 0 && _selection->selected(row));

        if (cell_info.selected)
        {
        	// Fill background with selected colour
    		Graphics g;
    		g.set_colour(Colour::black);
    		g.fill_rect(cell_info.screen.x, cell_info.screen.y,
    				cell_info.screen.x + _width -1,
    				cell_info.screen.y + _height - 1);
        }

        _item_renderer->render(cell_info);

        cell_info.bounds.max.y = cell_info.bounds.min.y;
    }
}

/**
 * Find the index to insert a row item based on the screen
 * coordinate
 *
 * @param y position to find the index for in screen co-ordinates
 * @return index for insert (returns row_count() if below last item).
 */
unsigned int ListView::insert_row_from_y(int y) const
{
	WindowState state;
	_window.get_state(state);

	int work_y = state.visible_area().work_y(y) + _margin.top;
	unsigned int row = (-work_y + _height/2) / _height;

	if (row < 0) row = 0;
	else if (row > _rows) row = _rows;

	return row;
}

/**
 * Get item index for the screen location (if any)
 *
 * @param y position on screen (screen coordinates)
 * @return index of item under location or ListView::invalid if no item at location.
 */
unsigned int ListView::screen_row(int y) const
{
	WindowState state;
	_window.get_state(state);

	int work_y = state.visible_area().work_y(y);
	work_y += _margin.top;

	unsigned int row = -work_y / _height;

	if (row < 0 && row >= row_count()) row = invalid;

	return row;
}

/**
 * Get the row and column to a screen location.
 *
 * This can be outside of the data area.
 *
 * @param scr_pt position on screen (screen coordinates)
 * @param row update with the row for the screen position or
 *        ListView::invalid if in top margin.
 *        == row_count() if in bottom margin.
 * @returns true if location is in the data
 */
bool ListView::screen_location(const Point &scr_pt, unsigned int &row) const
{
	WindowState state;
	_window.get_state(state);

	Point work = state.visible_area().work(scr_pt);

	work.x -= _margin.left;
	work.y += _margin.top;

	row = -work.y / _height;
	if (row < 0) row = invalid;
	else if (row > row_count()) row = row_count();

	return (work.x >= 0 && work.x < (int)_width
		&& row >= 0 && row < row_count());
}

/**
 * Get the bounds for the specified row
 *
 * @param row row to retrieve the bounds for
 * @param bounds bounding box to update (work area coordinates)
 */
void ListView::get_row_bounds(unsigned int row, BBox &bounds)
{
	bounds.min.x = _margin.left;
	bounds.max.x = bounds.min.x + _width;
	bounds.max.y = -_margin.top - row * _height;
	bounds.min.y = bounds.max.y - _height;
}

/*
 * Use UpdateWindow to update just the one column in a row
 */
void ListView::update_row(unsigned int row)
{
	if (row < 0 || row >= row_count()) return;

	BBox bounds;
	get_row_bounds(row, bounds);
	_window.update(bounds, 0);
}

/**
 * Mouse clicked on window containing list view
 */
void ListView::mouse_click(MouseClickEvent &event)
{
	if (_selection)
	{
		if (event.is_adjust() || event.is_select())
		{
			WindowState state;
			_window.get_state(state);

			Point work = state.visible_area().work(event.point());

			work.x -= _margin.left;
			work.y =  -work.y - _margin.top;
			if (work.x >= 0 && work.x < (int)_width
					&& work.y >= 0
				)
			{
				unsigned int row = work.y / row_height();
				if (row < _rows)
				{
					if (event.is_select()) _selection->select(row);
					else _selection->toggle(row);
				}
			}
		}
	}
}

/**
 * Current selection has changed
 */
void ListView::selection_changed(const SelectionChangedEvent &event)
{
	BBox bounds;
	// Currently only supporting single selection
	get_row_bounds(event.first(), bounds);
	_window.force_redraw(bounds);
}
