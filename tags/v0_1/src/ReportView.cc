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
 * ReportView.cc
 *
 *  Created on: 18-Mar-2009
 *      Author: alanb
 */

#include "ReportView.h"
#include "tbx/font.h"
#include "tbx/graphics.h"

using namespace tbx;

/**
 * Construct a report view for a window.
 *
 * Adds listeners it needs to the windows
 */
ReportView::ReportView(Window window) : _window(window), _rows(0),
    _height(32), _width(0), _column_gap(4),
    _selection(0)
{
	_window.add_redraw_listener(this);
}

/**
 * Destructor of report view
 *
 * removes any window listeners it used
 */
ReportView::~ReportView()
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
void ReportView::row_height(unsigned int height)
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
 * Change the gap between columns
 */
void ReportView::column_gap(unsigned int gap)
{
	if (_column_gap != gap)
	{
	    if (column_count() > 1)
	    {
	       if (_column_gap < gap) _width += (column_count() - 1) * (gap-_column_gap);
	       else _width -= (column_count() - 1) * (_column_gap - gap);
	    }
		_column_gap = gap;
		if (_rows && column_count())
		{
			update_window_extent();
			refresh();
		}
	}
}

/**
 * Set the selection model to use for the report view
 */
void ReportView::selection_model(SingleSelection *selection)
{
	if (selection == _selection) return;

	if (_selection)
	{
		_selection->remove_listener(this);
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
void ReportView::update_window_extent()
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

void ReportView::refresh()
{
	BBox all(_margin.left, -_margin.top - _rows * _height,
			_margin.left + _width, -_margin.top);
	_window.force_redraw(all);
}

/**
 * Change the margin
 */
void ReportView::margin(const tbx::Margin &margin)
{
	_margin = margin;
	update_window_extent();
	refresh();
}

/**
 * Add a column to the report view
 *
 * @param cr Renderer to render cells for the column
 * @param width the width of the column
 * @returns column number
 */
unsigned int ReportView::add_column(ItemRenderer *cr, unsigned int width)
{
	ColInfo ci;
	ci.renderer = cr;
	ci.width = width;
	if (column_count()) _width += _column_gap;
	_columns.push_back(ci);
	_width += width;
	update_window_extent();
	return column_count()-1;
}

/**
 * Remove column from report
 */
void ReportView::remove_column(unsigned int column)
{
	if (column >= column_count()) return;
	_width -= _columns[column].width;
	_columns.erase(_columns.begin() + column);
	if (column_count()) _width -= _column_gap;
	update_window_extent();

	refresh(); // TODO: Use window block copy instead
}

/**
 * Set the column width
 */
void ReportView::column_width(unsigned int column, unsigned int width)
{
	if (column >= column_count()) return;

	if (_columns[column].width != width)
	{
	    if (_columns[column].width > width) _width -= _columns[column].width - width;
	    else _width += width - _columns[column].width;

		_columns[column].width = width;
		//TODO: remove gap for zero width columns
		if (_rows)
		{
			update_window_extent();
			refresh(); //TODO: Use window block copy instead
		}
	}
}

/**
 * Returns column width.
 *
 * @param column 0 based column to return width for
 * @returns column width or 0 if column >= column_count()
 */
unsigned int ReportView::column_width(unsigned int column) const
{
	if (column < column_count()) return _columns[column].width;
	return 0;
}

/**
 * Size column to content.
 * Does nothing if there is no content
 */
void ReportView::autosize_column(unsigned int column, unsigned int min_width/* = 0*/, unsigned int max_width /*= 0xFFFFFFFF*/)
{
	if (column >= column_count() || _rows == 0) return;
	unsigned int col_width = min_width;
	ItemRenderer *renderer = _columns[column].renderer;

	for (unsigned int row = 0; row < _rows; row++)
	{
		unsigned int size = renderer->width(row);
		if (size > max_width) size = max_width;
		if (size > col_width) col_width = size;
	}

	if (col_width != _columns[column].width)
		column_width(column, col_width);
}

/**
 * Call after inserting rows into the collection
 * the ReportView is showing.
 *
 */
void ReportView::inserted(unsigned int where, unsigned int how_many)
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
 * the ReportView is showing.
 */
void ReportView::removed(unsigned int where, unsigned int how_many)
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
 * the ReportView is showing.
 */
void ReportView::changed(unsigned int where, unsigned int how_many)
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
void ReportView::cleared()
{
	if (_rows)
	{
		refresh();
		_rows = 0;
		if (_selection) _selection->clear();
		update_window_extent();
	}
}

/**
 * Get column from position on window
 */
unsigned int ReportView::column_from_x(int x) const
{
	int cc = column_count();
	int col = 0;
	x -= _margin.left;
	while (x > 0 && col < cc)
	{
		x -= _columns[col].width + _column_gap;
		if (x > 0) col++;
	}

	return col;
}

/**
 * Return x coordinate relative to window of column
 */
int ReportView::x_from_column(unsigned int column) const
{
	int x = _margin.left;
	unsigned int col = 0;

	while (col < column)
	{
		x += _columns[col].width + _column_gap;
		col++;
	}

	return x;
}

/**
 * Redraw the window
 */
void ReportView::redraw(const RedrawEvent &event)
{
	BBox work_clip = event.visible_area().work(event.clip());

	unsigned int first_row = (-work_clip.max.y - _margin.top) / _height;
	unsigned int last_row =  (-work_clip.min.y - _margin.top) / _height;

	if (first_row < 0) first_row = 0;
	if (last_row < 0) return; // Nothing to draw

	if (first_row >= _rows) return; // Nothing to draw
	if (last_row >= _rows) last_row = _rows - 1;

	unsigned int first_col = column_from_x(work_clip.min.x);
	unsigned int last_col = column_from_x(work_clip.max.x);

    if (first_col >= column_count()) return; // Nothing to redraw
    if (last_col >= column_count()) last_col = column_count() - 1;

    ItemRenderer::Info cell_info(event);

	cell_info.bounds.max.y = -first_row * _height - _margin.top;
	cell_info.screen.y = event.visible_area().screen_y(cell_info.bounds.max.y);
	int first_col_x = x_from_column(first_col);
	int first_col_scr_x = event.visible_area().screen_x(first_col_x);

    for (unsigned int row = first_row; row <= last_row; row++)
    {
        cell_info.bounds.min.y = cell_info.bounds.max.y - _height;
        cell_info.bounds.min.x = first_col_x;
        cell_info.screen.y -= _height;
        cell_info.screen.x = first_col_scr_x;
        cell_info.index = row;

        cell_info.selected = (_selection != 0 && _selection->is_selected(row));

        if (cell_info.selected)
        {
        	// Fill background with selected colour
        	int sel_right;
        	if (last_col < column_count()-1) sel_right = x_from_column(last_col+1);
        	else sel_right = _width + _margin.left;
    		Graphics g;
    		g.set_colour(Colour::black);
    		g.fill_rect(first_col_scr_x, cell_info.screen.y,
    				first_col_scr_x + sel_right - first_col_x - 1,
    				cell_info.screen.y + _height - 1);
        }

        for (unsigned int col = first_col; col <= last_col; col++)
        {
			cell_info.bounds.max.x = cell_info.bounds.min.x + _columns[col].width;
        	if (_columns[col].width > 0)
        	{
				_columns[col].renderer->render(cell_info);
        	}
			cell_info.screen.x += cell_info.bounds.max.x - cell_info.bounds.min.x + _column_gap;
			cell_info.bounds.min.x = cell_info.bounds.max.x + _column_gap;
        }
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
unsigned int ReportView::insert_row_from_y(int y) const
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
 * @return index of item under location or ReportView::invalid if no item at location.
 */
unsigned int ReportView::screen_row(int y) const
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
 * @param column update with the column for the screen position or
 *        ReportView::invalid if in left margin
 *        == columns_per_row() if in right margin
 * @param row update with the row for the screen position or
 *        ReportView::invalid if in top margin.
 *        == row_count() if in bottom margin.
 * @returns true if location is in the report
 */
bool ReportView::screen_location(const Point &scr_pt, unsigned int &column, unsigned int &row) const
{
	WindowState state;
	_window.get_state(state);

	Point work = state.visible_area().work(scr_pt);

	work.x -= _margin.left;
	work.y += _margin.top;

	if (work.x < 0) column = invalid;
	else
	{
		column = 0;
		unsigned int cc = column_count();
		while (column < cc && work.x >= 0)
		{
			work.x -= _columns[column].width;
			column++;
		}
	}
	row = -work.y / _height;
	if (row < 0) row = invalid;
	else if (row > row_count()) row = row_count();

	return (column >= 0 && column < column_count()
		&& row >= 0 && row < row_count());
}

/**
 * Get the bounds for the specified row
 *
 * @param row row to retrieve the bounds for
 * @param bounds bounding box to update (work area coordinates)
 */
void ReportView::get_row_bounds(unsigned int row, BBox &bounds)
{
	bounds.min.x = _margin.left;
	bounds.max.x = bounds.min.x + _width;
	bounds.max.y = -_margin.top - row * _height;
	bounds.min.y = bounds.max.y - _height;
}

/*
 * Use UpdateWindow to update just the one column in a row
 */
void ReportView::update_row(unsigned int row)
{
	if (row < 0 || row >= row_count()) return;

	BBox bounds;
	get_row_bounds(row, bounds);
	_window.update(bounds, 0);
}


/**
 * Get the cell bounds for the specified column and row
 *
 * It is permitted to specifiy a column and row outside of the
 * used cells.
 *
 * @param column column to retrieve the bounds for
 * @param row row to retrieve the bounds for
 * @param bounds bounding box to update (work area coordinates)
 */
void ReportView::get_cell_bounds(unsigned int column, unsigned int row, BBox &bounds)
{
	bounds.min.x = x_from_column(column);
	bounds.max.x = bounds.min.x + _columns[column].width;
	bounds.max.y = -_margin.top - row * _height;
	bounds.min.y = bounds.max.y - _height;
}

/*
 * Use UpdateWindow to update just the one column in a row
 */
void ReportView::update_cell(unsigned int column, unsigned int row)
{
	if (column < 0 || column >= column_count()) return;
	if (row < 0 || row >= row_count()) return;

	BBox bounds;
	get_cell_bounds(column, row, bounds);
	_window.update(bounds, 0);
}

/**
 * Mouse clicked on window containing report view
 */
void ReportView::mouse_click(MouseClickEvent &event)
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
void ReportView::selection_changed(unsigned int old_index, unsigned int new_index)
{
	BBox bounds;
	if (old_index != SingleSelection::none)
	{
		get_row_bounds(old_index, bounds);
		_window.force_redraw(bounds);
	}
	if (new_index != SingleSelection::none)
	{
		get_row_bounds(new_index, bounds);
		_window.force_redraw(bounds);
	}
}


