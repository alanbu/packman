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
 * RecommendsWindow.cc
 *
 *  Created on: 26 Sep 2013
 *      Author: alanb
 */

#include "RecommendsWindow.h"
#include "Packages.h"
#include "PackageConfigWindow.h"
#include "libpkg/pkgbase.h"

#include "tbx/application.h"
#include "tbx/button.h"
#include "tbx/actionbutton.h"
#include "tbx/optionbutton.h"
#include "tbx/deleteonhidden.h"
#include "tbx/res/reswindow.h"

#include <iostream>

// Gap between buttons
const int ygap = 8;

RecommendsWindow::RecommendsWindow(std::vector<std::string> &recommends, std::vector<std::string> &suggests) :
  _window("Recommends"),
  _add_command(this, &RecommendsWindow::add)
{
	int opt_id = 16;
	tbx::res::ResWindow res_pack_wnd = tbx::app()->resource("Recommends");
	tbx::res::ResOptionButton res_option = res_pack_wnd.gadget(opt_id);
	tbx::Button recommends_text = _window.gadget(0);
	tbx::Button suggests_text = _window.gadget(1);
	int button_move_y = 0;

	if (recommends.empty())
	{
		// Remove recommends button and replace with suggests
		tbx::BBox bounds = recommends_text.bounds();
		button_move_y += bounds.height();
		_window.remove_gadget(0);
		suggests_text.bounds(bounds);
	} else
	{
		for (std::vector<std::string>::iterator i = recommends.begin(); i != recommends.end(); ++i)
		{
			add_option(*i, res_option, button_move_y);
		}
	}

	if (suggests.empty())
	{
		tbx::BBox bounds = suggests_text.bounds();
		button_move_y += bounds.height();
		_window.remove_gadget(1);
	} else
	{
		if (!recommends.empty())
		{
			tbx::BBox bounds = suggests_text.bounds();
			int suggests_height = bounds.height();
			bounds.max.y = res_option.ymin() - ygap;
			bounds.min.y = bounds.max.y - suggests_height;
			suggests_text.bounds(bounds);
            res_option.ymax(res_option.ymax() - suggests_height - ygap * 2);
            res_option.ymin(res_option.ymin() - suggests_height - ygap * 2);
		}
		for (std::vector<std::string>::iterator i = suggests.begin(); i != suggests.end(); ++i)
		{
			add_option(*i, res_option, button_move_y);
		}
	}

    tbx::ActionButton cancel_button(_window.gadget(2));
    cancel_button.move_by(0, button_move_y);
	tbx::ActionButton add_button(_window.gadget(3));
	add_button.move_by(0, button_move_y);
	add_button.add_select_command(&_add_command);
	// Dispose of object when it is hidden
	_window.add_has_been_hidden_listener(new tbx::DeleteClassOnHidden<RecommendsWindow>(this));

	tbx::BBox add_bounds = add_button.bounds();

	tbx::BBox extent(0,add_bounds.min.y - ygap, add_bounds.max.x + ygap, 0);
	_window.extent(extent);

	tbx::ShowFullSpec loc;
	loc.wimp_window = tbx::ShowFullSpec::WINDOW_SHOW_TOP;
	loc.visible_area.scroll().x = 0;
	loc.visible_area.scroll().y = 0;
	loc.visible_area.bounds().size(extent.width(), extent.height());
	_window.show(loc);
}

RecommendsWindow::~RecommendsWindow()
{
	_window.delete_object();
}

/**
 * Add package to recommendation windows list
 *
 * @param pkgname   name of package to add
 * @param res_option template for option button to add the package (updated to last option details)
 * @param button_move_y distance to move following buttons (updated to move down by space taken by the option)
 */
void RecommendsWindow::add_option(std::string pkgname, tbx::res::ResOptionButton &res_option, int &button_move_y)
{
    pkg::pkgbase * package_base = Packages::instance()->package_base();
    const pkg::binary_control &bctrl = package_base->control()[pkgname];
	std::string desc(pkgname);

    desc+= " ";
    desc+= bctrl.version();
    desc+= " ";
	desc += bctrl.short_description();
	if (desc.size() > 79) desc.erase(79);

	if (_packages.empty())
	{
		// First option is already on the window
		tbx::OptionButton opt_button = _window.gadget(16);
		opt_button.label(desc);
	} else
	{
		// Add new button based on res_option template
		res_option.component_id(res_option.component_id()+1);
		int height = res_option.ymax() - res_option.ymin();
		res_option.ymax(res_option.ymin() - ygap);
		res_option.ymin(res_option.ymax() - height);
		res_option.label(desc);
		_window.add_gadget(res_option);
		button_move_y -= height + ygap;
	}

	_packages.push_back(pkgname);
}


/**
 * Add all selected packages to the install window
 */
void RecommendsWindow::add()
{
	int num_packages = _packages.size();
	std::vector<std::string> selected;
	for (int i = 0; i < num_packages; ++i)
	{
		tbx::OptionButton opt_button = _window.gadget(i+16);
		if (opt_button.on()) selected.push_back(_packages[i]);
	}
	if (!selected.empty())
	{
		Packages::instance()->select_install(selected);
		PackageConfigWindow::update();
		PackageConfigWindow::bring_to_top();
	}
}
