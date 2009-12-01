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
 * ListView.h
 *
 *  Created on: 24-Jul-2009
 *      Author: alanb
 */

#ifndef LISTVIEW_H_
#define LISTVIEW_H_

#include "tbx/window.h"
#include "tbx/redrawlistener.h"
#include "tbx/mouseclicklistener.h"
#include "tbx/margin.h"
#include "itemrenderer.h"
#include "singleselection.h"


/**
 * Report view
 */
class ListView :
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
	SingleSelection *_selection;
	ItemRenderer *_item_renderer;

public:
	ListView(tbx::Window window, ItemRenderer *item_renderer = 0);
	virtual ~ListView();

	static const unsigned int invalid = -1;

	void item_renderer(ItemRenderer *ir);

	void selection_model(SingleSelection *selection);
	SingleSelection *selection_model() const {return _selection;}

	void update_window_extent();

	const tbx::Margin &margin() const {return _margin;}
	void margin(const tbx::Margin &margin);

	void row_height(unsigned int height);
	unsigned int row_height() const {return _height;}

	unsigned int row_count() const {return _rows;}

	void width(unsigned int width);
	unsigned int width() const;

	void autosize(unsigned int min_width = 0, unsigned int max_width = 0xFFFFFFFF);

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

	unsigned int insert_row_from_y(int y) const;
	unsigned int screen_row(int y) const;
	bool screen_location(const tbx::Point &scr_pt, unsigned int &row) const;
	void get_row_bounds(unsigned int row, tbx::BBox &bounds);
	void update_row(unsigned int row);
};

#endif /* LISTVIEW_H_ */
