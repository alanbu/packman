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
 * ItemRenderer.cc
 *
 *  Created on: 25-Mar-2009
 *      Author: alanb
 */

#include "ItemRenderer.h"
#include "tbx/font.h"
#include "tbx/sprite.h"

using namespace tbx;

/**
 * Render as cell retrieved as text.
 */
void WimpFontItemRenderer::render(const ItemRenderer::Info &info)
{
	std::string t = text(info.index);
	if (!t.empty())
	{
		WimpFont font;

		if (info.selected)
		{
			font.set_font_colours(Colour::white, Colour::black);
		} else
		{
			font.set_font_colours(Colour::black, Colour::white);
		}
		font.paint(info.screen.x, info.screen.y+8, t);
	}
}

/**
 * Used to measure the width of a column
 */
unsigned int WimpFontItemRenderer::width(unsigned int index) const
{
	std::string t = text(index);
	if (t.empty()) return 0;

	WimpFont font;
	return font.string_width_os(t);
}


/**
 * Render a sprite in the cell
 */
void SpriteItemRenderer::render(const ItemRenderer::Info &info)
{
	tbx::Sprite *s = sprite(info.index);
	if (s) s->plot_screen(info.screen);
}

/**
 * Used to measure the width of a column
 */
unsigned int SpriteItemRenderer::width(unsigned int row) const
{
	tbx::Sprite *s = sprite(row);
	return s ? s->width() : 0;
}

/**
 * Used to measure the height
 */
unsigned int SpriteItemRenderer::height(unsigned int row) const
{
	tbx::Sprite *s = sprite(row);
	return s ? s->height() : 0;
}
