/*********************************************************************
* This file is part of PackMan
*
* Copyright 2009-2020 AlanBuckley
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
 * InfoWindow.h
 *
 *  Created on: 24-Jul-2009
 *      Author: alanb
 */

#ifndef INFOWINDOW_H_
#define INFOWINDOW_H_

#include "tbx/window.h"
#include "tbx/displayfield.h"
#include "tbx/textarea.h"
#include "tbx/scrolllist.h"
#include "tbx/actionbutton.h"
#include "tbx/uri.h"
#include "libpkg/binary_control.h"
#include "Commands.h"

/**
 * Class to handle window showing information on the
 * selected package
 */
class InfoWindow : public ISelectedPackage,
    public tbx::URIResultHandler
{
	tbx::Window _window;
	tbx::DisplayField _installed;
	tbx::TextArea _description;
	tbx::ScrollList _components;
	tbx::ActionButton _install_button;
	tbx::ActionButton _remove_button;
	tbx::ActionButton _components_button;
	tbx::DisplayField _homepage;
	tbx::ActionButton _web_button;
	InstallCommand _install_command;
	RemoveCommand _remove_command;
	CopyrightCommand _copyright_command;
	tbx::CommandMethod<InfoWindow> _web_command;
	std::string _package_name;
	tbx::URI _uri;
	static InfoWindow *_instance;
public:
	InfoWindow();
	virtual ~InfoWindow();

	static void show(const pkg::binary_control *ctrl);
	static void show(const std::string pkgname);
	void update_details(const pkg::binary_control *ctrl);

	// ISelectedPackage interface
	virtual const pkg::binary_control *selected_package();

private:
    void show_homepage();
	// URIResultHandler callback
    virtual void uri_result(tbx::URI &uri, bool claimed);
};

#endif /* INFOWINDOW_H_ */
