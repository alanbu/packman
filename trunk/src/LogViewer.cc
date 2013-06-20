/*********************************************************************
* Copyright 2013 Alan Buckley
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
 * LogViewer.cc
 *
 *  Created on: 14 Jun 2013
 *      Author: alanb
 */

#include "LogViewer.h"
#include "tbx/deleteonhidden.h"
#include "tbx/font.h"
#include "tbx/menu.h"
#include <fstream>
#include <iostream>

static tbx::Colour log_colour[3] = { tbx::Colour(255,0,0), tbx::Colour(255,127,39), tbx::Colour(0,128,0) };


/**
 * Construct a log viewer for the given log
 *
 * @param log log to show
 */
LogViewer::LogViewer(std::tr1::shared_ptr<pkg::log> log) :
     _window("Log"),
     _log(log),
     _log_entry_renderer(log),
     _view(_window, &_log_entry_renderer)
{
	_view.inserted(0, log->size());
	_window.add_has_been_hidden_listener(new tbx::DeleteClassOnHidden<LogViewer>(this));

	// Set up for saving of log
	tbx::Menu menu = _window.menu();
	tbx::MenuItem saveas_item =  menu.item(0);
	tbx::SaveAs saveas = saveas_item.click_show();

	saveas.set_save_to_file_handler(this);
}

/**
 * Delete log viewer, deleting window and log file if option is set
 */
LogViewer::~LogViewer()
{
	_window.delete_object();
}

/**
 * Show the log viewer window
 */
void LogViewer::show()
{
	_window.show();
}

/**
 * Save the log to file from saveas
 */
void LogViewer::saveas_save_to_file(tbx::SaveAs saveas, bool selection, std::string file_name)
{
	std::cout << "Saving log to " << file_name << std::endl;
   std::ofstream file(file_name.c_str());
   file << *(_log.get());
   saveas.file_save_completed(file.good(), file_name);
}

/**
 * Render a log item in the viewer
 *
 * @param info - information on what to render where.
 */
void LogViewer::LogItemRenderer::render(const tbx::view::ItemRenderer::Info &info)
{
   const pkg::log_entry &item = _log->entry(info.index);
   std::string t = item.when_text() + " " + item.text();
   if (!t.empty())
   {
	  tbx::WimpFont font;

	  if (info.selected)
	  {
		 font.set_colours(tbx::Colour::white, tbx::Colour::black);
	  } else if (item.type() < 3)
      {
		 font.set_colours(log_colour[item.type()], tbx::Colour::white);
      } else
	  {
		 font.set_colours(tbx::Colour::black, tbx::Colour::white);
	  }
	  font.paint(info.screen.x, info.screen.y+8, t);
   }
}


/**
 * Get width of log item in OS units
 *
 * @param index of item to measure
 * @returns width of rendered item at index
 */
unsigned int LogViewer::LogItemRenderer::width(unsigned int index) const
{
	std::string t = _log->entry(index).when_text() + " " + _log->entry(index).text();
	if (t.empty()) return 0;

	tbx::WimpFont font;
	return font.string_width_os(t);
}

/**
* log entry is height of the Wimp font which is always 40
*
* @param index (unused in this renderer)
*/
unsigned int LogViewer::LogItemRenderer::height(unsigned int index) const
{
	return 40;
}

/**
 *  Called to get the size of an item.
 */
tbx::Size LogViewer::LogItemRenderer::size(unsigned int index) const
{
	return tbx::Size((int)width(index), 40);
}
