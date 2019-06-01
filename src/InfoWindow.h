/*********************************************************************
* Copyright 2009-2019 Alan Buckley
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
