/*********************************************************************
* This file is part of PackMan
*
* Copyright 2013-2020 AlanBuckley
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
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
