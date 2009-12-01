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
 * ReportView.h
 *
 *  Created on: 18-Mar-2009
 *      Author: alanb
 */

#ifndef REPORTVIEW_H_
#define REPORTVIEW_H_

#include "tbx/window.h"
#include "tbx/redrawlistener.h"
#include "tbx/mouseclicklistener.h"
#include "tbx/margin.h"
#include "itemrenderer.h"
#include "singleselection.h"

#include <vector>


/**
 * Report view
 */
class ReportView :
	public tbx::RedrawListener,
	public tbx::MouseClickListener,
	public SingleSelectionListener
{
private:
	tbx::Window _window;
	tbx::Margin _margin;
	unsigned int _rows;
	unsigned int _height;
	unsigned int _width;
	unsigned int _column_gap;
	SingleSelection *_selection;

	struct ColInfo
	{
		ItemRenderer *renderer;
		unsigned int width;
	};
	std::vector<ColInfo> _columns;

public:
	ReportView(tbx::Window window);
	virtual ~ReportView();

	static const unsigned int invalid = -1;

	void selection_model(SingleSelection *selection);
	SingleSelection *selection_model() const {return _selection;}

	void update_window_extent();

	const tbx::Margin &margin() const {return _margin;}
	void margin(const tbx::Margin &margin);

	void row_height(unsigned int height);
	unsigned int row_height() const {return _height;}

	void column_gap(unsigned int gap);
	unsigned int column_gap() const {return _column_gap;}

	unsigned int add_column(ItemRenderer *cr, unsigned int width);
	//TODO: void insert_column(int where, ReportViewCellRenderer *cr, int width);
	void remove_column(unsigned int column);

	unsigned int column_count() const {return _columns.size();}
	unsigned int row_count() const {return _rows;}

	void column_width(unsigned int column, unsigned int width);
	unsigned int column_width(unsigned int column) const;

	void autosize_column(unsigned int column, unsigned int min_width = 0, unsigned int max_width = 0xFFFFFFFF);

	// Redraw listener override
	virtual void redraw(const tbx::RedrawEvent &event);

	// Mouse click listener override
	virtual void mouse_click(tbx::MouseClickEvent &event);

	// Single selection listener override
	virtual void selection_changed(unsigned int old_index, unsigned int new_index);

	// Calls to update view
	void refresh();
	virtual void inserted(unsigned int where, unsigned int how_many);
	virtual void removed(unsigned int where, unsigned int how_many);
	void changed(unsigned int where, unsigned int how_many);
	void cleared();
	void cell_changed(int column, int row);

	unsigned int column_from_x(int x) const;
	int x_from_column(unsigned int column) const;
	unsigned int insert_row_from_y(int y) const;
	unsigned int screen_row(int y) const;
	bool screen_location(const tbx::Point &scr_pt, unsigned int &column, unsigned int &row) const;
	void get_row_bounds(unsigned int row, tbx::BBox &bounds);
	void update_row(unsigned int row);
	void get_cell_bounds(unsigned int column, unsigned int row, tbx::BBox &bounds);
	void update_cell(unsigned int column, unsigned int row);
};

#endif /* REPORTVIEW_H_ */
