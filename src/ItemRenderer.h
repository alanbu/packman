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
 * ItemRenderer.h
 *
 *  Created on: 25-Mar-2009
 *      Author: alanb
 */

#ifndef ITEMRENDERER_H_
#define ITEMRENDERER_H_

#include <string>
#include "tbx/bbox.h"
#include "tbx/redrawlistener.h"

/**
 * Base class to render an indexed item.
 *
 */
class ItemRenderer
{
public:
	ItemRenderer() {};
	virtual ~ItemRenderer() {};

public:
	/**
	 * Information on what needs to be redrawn
	 */
	struct Info
	{
		/**
		 * Redraw event object from FixedCellView
		 */
		const tbx::RedrawEvent &redraw;

		/**
		 * Bounding box of cell to redraw in work area coordinates
		 */
		tbx::BBox bounds;

		/**
		 * Bottom left of items bounding box in screen coordinates
		 */
		tbx::Point screen;

		/**
		 * zero based index of item to redraw
		 */
		unsigned int index;

		/**
		 * Is the item selected
		 */
		bool selected;

		/**
		 * Construct from Redraw event.
		 *
		 * Other parameters need to be set before it is used.
		 */
		Info(const tbx::RedrawEvent &r) : redraw(r) {};
	};

	/**
	 * Called to render each item that needs drawing
	 *
	 * The item is only clipped to the containing Windows visible area,
	 * not to the cell bounds so the content must be drawn entirely
	 * inside the cell bounding box or the clip area should be set
	 * before drawing.
	 *
	 * @param info Information on redraw event and item to be redrawn
	 */
	virtual void render(const ItemRenderer::Info &info) = 0;

	/**
	 * Used to measure the width of a column
	 */
	virtual unsigned int width(unsigned int index) const = 0;

	/**
	 * Used to measure the height of column
	 */
	virtual unsigned int height(unsigned int index) const = 0;
};

/**
 * Renderer base class to put text in a cell using the current
 * wimp font.
 */
class WimpFontItemRenderer : public ItemRenderer
{
public:
	virtual ~WimpFontItemRenderer() {};

	/**
	 * Render text in black using current wimp font
	 */
	virtual void render(const ItemRenderer::Info &info);

	/**
	 * Override this to provide the text for a cell
	 */
	virtual std::string text(unsigned int index) const = 0;

	/**
	 * Used to measure the width of a column
	 */
	virtual unsigned int width(unsigned int row) const;

	/**
	 * Wimp font height is always 32
	 */
	virtual unsigned int height(unsigned int row) const {return 32;}
};

namespace tbx
{
   class Sprite;
};

/**
 * Item renderer to render a sprite in a cell
 */

class SpriteItemRenderer : public ItemRenderer
{
public:
	virtual ~SpriteItemRenderer() {};

	/**
	 * Render the sprite to the screen.
	 */
	virtual void render(const ItemRenderer::Info &info);

	/**
	 * Override this to provide the sprite for a cell
	 */
	virtual tbx::Sprite *sprite(unsigned int index) const = 0;

	/**
	 * Used to measure the width of a column
	 */
	virtual unsigned int width(unsigned int row) const;

	/**
	 * Used to measure the height
	 */
	virtual unsigned int height(unsigned int row) const;
};

#endif /* ITEMRENDERER_H_ */
