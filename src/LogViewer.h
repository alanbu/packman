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
 * LogViewer.h
 *
 *  Created on: 14 Jun 2013
 *      Author: alanb
 */

#ifndef LOGVIEWER_H_
#define LOGVIEWER_H_

#include "tbx/window.h"
#include "tbx/view/listview.h"
#include "tbx/saveas.h"
#include "libpkg/log.h"
#include <tr1/memory>

/**
 * Class to show a libpkg log in a window
 */
class LogViewer : public tbx::SaveAsSaveToFileHandler
{
    tbx::Window _window;
    std::tr1::shared_ptr<pkg::log> _log;
    bool _delete_log;

    class LogItemRenderer : public tbx::view::ItemRenderer
    {
        std::tr1::shared_ptr<pkg::log> _log;

        public:
        LogItemRenderer(std::tr1::shared_ptr<pkg::log> render_log) : _log(render_log) {}
		virtual void render(const ItemRenderer::Info &info);
		virtual unsigned int width(unsigned int index) const;
		virtual unsigned int height(unsigned int index) const;
		virtual tbx::Size size(unsigned int index) const;
    } _log_entry_renderer;

    tbx::view::ListView _view;

	void saveas_save_to_file(tbx::SaveAs saveas, bool selection, std::string file_name);

public:
   LogViewer(std::tr1::shared_ptr<pkg::log> log);
   ~LogViewer();

   void title(const std::string &title);

   void show();
};

#endif /* LOGVIEWER_H_ */
