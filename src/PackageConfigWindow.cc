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
 * PackConfigWindow.cc
 *
 *  Created on: 18 Apr 2013
 *      Author: alanb
 */

#include "PackageConfigWindow.h"
#include "Packages.h"
#include "Commands.h"
#include "AbbrevSize.h"
#include "RecommendsWindow.h"

#include "libpkg/pkgbase.h"
#include "libpkg/component_update.h"

#include "tbx/stringutils.h"
#include "tbx/button.h"
#include "tbx/path.h"
#include "tbx/draggable.h"
#include "tbx/actionbutton.h"
#include "tbx/optionbutton.h"
#include "tbx/application.h"
#include "tbx/messagewindow.h"
#include "tbx/res/reswindow.h"
#include "tbx/res/resbutton.h"
#include "tbx/res/resactionbutton.h"

#include <set>
#include <sstream>
#include <iostream>

PackageConfigWindow *PackageConfigWindow::_instance = 0;
int PackageConfigWindow::_show_x = 200;
int PackageConfigWindow::_show_y = 64;

// y margin/gap for package list
const int pack_gap = 8;

// Character used to separate apply button actions
const char apply_delim = '/';

PackageConfigWindow::PackageConfigWindow() : _window("PackConfig"),
	_path_chooser(this),
	_apply_command(this, &PackageConfigWindow::apply),
	_cancel_command(this, &PackageConfigWindow::cancel),
	_recommends_command(this, &PackageConfigWindow::recommends),
	_row_height(0),
	_top_margin(0)
{
	tbx::ActionButton apply_button = _window.gadget(APPLY_BUTTON);
	apply_button.add_select_command(&_apply_command);
	tbx::ActionButton cancel_button = _window.gadget(CANCEL_BUTTON);
	cancel_button.add_select_command(&_cancel_command);
	tbx::ActionButton recommends_button = _window.gadget(RECOMMEND_BROWSE);
	recommends_button.add_select_command(&_recommends_command);

	tbx::DisplayField ft = _window.gadget(COMPONENT_AREA);
	_no_component_text = ft.text();
	std::string button_text = apply_button.text();
	std::string::size_type delim1_pos = button_text.find(apply_delim);
	std::string::size_type delim2_pos = button_text.find(apply_delim,delim1_pos+1);
	_install_text = button_text.substr(0, delim1_pos);
	_upgrade_text = button_text.substr(delim1_pos+1, delim2_pos - delim1_pos - 1);
	_remove_text = button_text.substr(delim2_pos+1);
}

PackageConfigWindow::~PackageConfigWindow()
{
	_window.delete_object();
	_instance = 0;
}

/**
 * Update package configuration window
 *
 * Shows the window if it has not already been shown
 */
void PackageConfigWindow::update()
{
	bool show_wnd = (_instance == 0);
	try
	{
		if (_instance == 0) _instance = new PackageConfigWindow();
		_instance->package_added();
	} catch(std::exception &e)
	{
		tbx::show_message(e.what(), "Exception while trying to update the package window");
	}

	if (show_wnd)
	{
		tbx::ShowFullSpec loc;
		loc.wimp_window = tbx::ShowFullSpec::WINDOW_SHOW_TOP;
		loc.visible_area.scroll().x = 0;
		loc.visible_area.scroll().y = 0;
		loc.visible_area.bounds() = _instance->_window.extent();
		loc.visible_area.bounds().move_to(_show_x, _show_y);
		_instance->_window.show(loc);
	}
}

/**
 * Bring window to the top of the window stack
 */
void PackageConfigWindow::bring_to_top()
{
	if (_instance) _instance->show();
}

/**
 * Start installation
 */
void PackageConfigWindow::apply()
{
	try
	{
		pkg::component_update comp_update(Packages::instance()->package_base()->component_update_pathname());
		int i = 0;
		bool dum_path_set; // Needed for call below but not used
		for (std::vector<pkg::component>::iterator c = _components.begin(); c != _components.end(); ++c)
		{
			get_component(i++, *c, dum_path_set);
			comp_update.insert(*c);
		}
		comp_update.commit();

		CommitCommand commit;
		commit.execute();
		delete this;
	} catch (std::exception &e)
	{
		tbx::show_message(std::string("Failed to start the configuration.\nError: ") + e.what());
	}
}

/**
 *
 */
void PackageConfigWindow::cancel()
{
	Packages::instance()->clear_selection();
	delete this;
}

/**
 * Show the recommendations window
 */
void PackageConfigWindow::recommends()
{
	std::vector< std::pair<std::string,std::string> > packages_to_check;
	std::vector< std::string> recs_found;
	std::vector< std::string> sugs_found;

	for(std::vector<PackageInfo>::iterator i = _packages.begin(); i != _packages.end(); ++i)
	{
		if (i->action != REMOVE && i->action != AUTO_REMOVE)
		{
			packages_to_check.push_back(std::make_pair(i->name,i->version));
		}
	}

	Packages::instance()->get_recommendations(packages_to_check, recs_found, sugs_found);

	new RecommendsWindow(recs_found, sugs_found);
}

/**
 * A package has been added to the list of selected packages
 * so update the window appropriately.
 */
void PackageConfigWindow::package_added()
{
	std::vector<PackageInfo> new_list;
	std::vector<pkg::component> new_comps;

	build_lists_from_selection(new_list, new_comps);

	if (new_list == _packages) return; // Nothing has changed

	tbx::DisplayField pack_area = _window.gadget(PACKAGE_AREA);
	tbx::BBox pack_area_bbox = pack_area.bounds();
	int pack_ymove = 0;
	int comp_ymove = 0;
	int pack_added_y = 0; // Only updated if new package gadgets are added

	pack_area.text("");

	unsigned int j = 0;
	while (j < new_list.size())
	{
		if (j < _packages.size())
		{
			if (_packages[j] != new_list[j])
			{
				tbx::Button pack_gadget = _window.gadget(FIRST_PACKAGE + (j<<1));
				tbx::ActionButton withdraw_button = _window.gadget(FIRST_PACKAGE + (j<<1) + 1);
				pack_gadget.value(new_list[j].display_text());
				if (!_packages[j].auto_action() && new_list[j].auto_action())
				{
					withdraw_button.fade(true);
				} else if (_packages[j].auto_action() && !new_list[j].auto_action())
				{
					withdraw_button.fade(false);
				}
				_packages[j] = new_list[j];
			}
		} else if (j == 0)
		{
			// The gadgets for the first package are always on the window
			tbx::Button pack_gadget = _window.gadget(FIRST_PACKAGE + (j<<1));
			tbx::ActionButton withdraw_button = _window.gadget(FIRST_PACKAGE + (j<<1) + 1);
			pack_gadget.value(new_list[j].display_text());
			if (new_list[j].auto_action()) withdraw_button.fade(true);
			withdraw_button.add_selected_listener(this);
		} else
		{
			tbx::res::ResWindow res_pack_wnd = tbx::app()->resource("PackConfig");
			tbx::res::ResButton res_pack_text = res_pack_wnd.gadget(FIRST_PACKAGE);
			tbx::res::ResActionButton res_pack_but = res_pack_wnd.gadget(FIRST_PACKAGE+1);
			int height = res_pack_text.ymax() - res_pack_text.ymin();
			pack_added_y = pack_area_bbox.max.y - pack_gap - (j * (pack_gap + height));

			res_pack_text.component_id(FIRST_PACKAGE + (j<<1));
			res_pack_but.component_id(FIRST_PACKAGE + (j<<1) +1);

			res_pack_text.value(new_list[j].display_text());
			if (new_list[j].auto_action()) res_pack_but.faded(true);

			res_pack_text.ymax(pack_added_y);
			res_pack_but.ymax(pack_added_y);
			pack_added_y -= height;
			res_pack_text.ymin(pack_added_y);
			res_pack_but.ymin(pack_added_y);

			_window.add_gadget(res_pack_text);
			_window.add_gadget(res_pack_but);

			tbx::ActionButton withdraw_button = _window.gadget(FIRST_PACKAGE + (j<<1) + 1);
			withdraw_button.add_selected_listener(this);
		}

		j++;
	}

	_packages = new_list;

	if (pack_added_y) pack_ymove = pack_added_y - pack_gap - pack_area_bbox.min.y;

	if (pack_ymove)
	{
		pack_area_bbox.min.y = pack_added_y - pack_gap;
		pack_area.bounds(pack_area_bbox);

		shift_below_pack_area(pack_ymove);
	}

	update_download_totals();

	if (!new_comps.empty())
	{
		if (new_comps.size() > _components.size())
		{
			// Need to add some more items
			tbx::DisplayField comp_area = _window.gadget(COMPONENT_AREA);
			tbx::res::ResWindow res_window = tbx::app()->resource("CompProto");
			comp_area.text("");
			tbx::BBox comp_bbox = comp_area.bounds();
			int row_y = comp_bbox.max.y;
			int x = comp_bbox.min.x;
//TODO: assert(res_window.num_gadgets() == COMP_COUNT)

			if (!_components.empty())
			{
				// Set start location for next component
				row_y = comp_bbox.min.y + _top_margin;

				// Need to add separator after last item
				int first_id = FIRST_COMPONENT + ((_components.size() - 1) << COMP_SHIFT);
				row_y += _row_height;
				tbx::res::ResGadget res_separator = res_window.gadget(COMP_SEPARATOR);
				res_separator.component_id(first_id + res_separator.component_id());
				res_separator.xmin(res_separator.xmin() + x);
				res_separator.xmax(res_separator.xmax() + x);
				res_separator.ymin(res_separator.ymin() + row_y);
				res_separator.ymax(res_separator.ymax() + row_y);
				_window.add_gadget(res_separator);
				row_y -= _row_height;
			}

			// Add new components to the end first
			for (int comp_no = (int)_components.size(); comp_no < (int)new_comps.size(); ++comp_no)
			{
				pkg::component &comp = new_comps[comp_no];
				int first_id = FIRST_COMPONENT + (comp_no << COMP_SHIFT);
				int lowest = 0;
				int highest = -1000;
				bool last_comp = (comp_no == (int)(new_comps.size()-1));

				// Add gadgets for new row
				for (tbx::res::ResWindow::const_gadget_iterator g = res_window.gadget_cbegin();
						g != res_window.gadget_cend(); ++g)
				{
					// Exclude last rule-off
					tbx::res::ResGadget res_gadget(*g);
					if (_row_height == 0)
					{
						 if (res_gadget.ymin() < lowest) lowest = res_gadget.ymin();
						 if (res_gadget.ymax() > highest) highest = res_gadget.ymax();
					}
					if (!last_comp || res_gadget.component_id() != COMP_SEPARATOR)
					{
						res_gadget.component_id(first_id + res_gadget.component_id());
						res_gadget.xmin(res_gadget.xmin() + x);
						res_gadget.xmax(res_gadget.xmax() + x);
						res_gadget.ymin(res_gadget.ymin() + row_y);
						res_gadget.ymax(res_gadget.ymax() + row_y);

						_window.add_gadget(res_gadget);
					}
				}

				tbx::Button comp_path = _window.gadget(first_id);
				tbx::DisplayField comp_leaf = _window.gadget(first_id + COMP_LEAF);
				tbx::Draggable comp_drag = _window.gadget(first_id + COMP_DRAGGABLE);
				_path_chooser.add_path_listeners(comp_no);

				int found_idx = find_component(comp.name());
				if (found_idx == -1)
				{
					tbx::Path path(Packages::instance()->package_base()->paths()(comp.name(),""));
					comp_path.value(path.parent());
					comp_leaf.text(path.leaf_name());
					if (_components.empty())
					{
						// Get path colours
						_default_path_colour = comp_path.foreground();
						_changed_path_colour = tbx::WimpColour::black;
					}
				} else
				{
					copy_component_values(found_idx, comp_no);
				}

				if (_row_height == 0)
				{
					_row_height = -lowest;
					_top_margin = -highest;
				}
				row_y -= _row_height;
			};

			row_y -= _top_margin;
			comp_ymove = row_y - comp_bbox.min.y;
			if (comp_ymove)
			{
				comp_bbox.min.y = row_y;
				comp_area.bounds(comp_bbox);
				for (int id = CANCEL_BUTTON; id <= APPLY_BUTTON; ++id)
				{
					tbx::Gadget g = _window.gadget(id);
					g.move_by(0, comp_ymove);
				}
			}
		}
		// Check and update existing rows
		update_existing_components(new_comps);
	}

	_components = new_comps;

	int ext_ychange = pack_ymove + comp_ymove;
	if (ext_ychange)
	{
		tbx::BBox extent = _window.extent();
		extent.min.y += ext_ychange;
		_window.extent(extent);
	}

	update_recomendations();
	update_apply_button();
}

/**
 * Remove package from window when "withdraw" button is clicked
 */
void PackageConfigWindow::button_selected(tbx::ButtonSelectedEvent &event)
{
	tbx::ActionButton pack_button = event.id_block().self_component();
	int package_idx = (pack_button.id() - FIRST_PACKAGE)>>1;
	if (package_idx == 0)
	{
		if (_packages.size() == 1 || _packages[1].auto_action())
		{
			tbx::show_message("You can't remove the last package this way.\nUse the cancel button instead.");
			return;
		}
	}
	remove_package_from_list(package_idx);
}

/**
 * Remove a package from the list of packages to configure
 *
 * Also removes any components that are no longer required
 *
 * @param package_idx index of package in current list of packages
 */
void PackageConfigWindow::remove_package_from_list(int package_idx)
{
	Packages::instance()->deselect(_packages[package_idx].name);
	package_removed();
}

/**
 * A package has been removed from the list of selected
 * packages.
 */
void PackageConfigWindow::package_removed()
{
	std::vector<PackageInfo> new_list;
	std::vector<pkg::component> new_comps;

	build_lists_from_selection(new_list, new_comps);
	if (new_list == _packages) return; // Nothing has changed

	tbx::Button pack_text;
	tbx::ActionButton pack_button;

	for (unsigned int idx = 0; idx < new_list.size(); ++idx)
	{
		if (new_list[idx].name != _packages[idx].name)
		{
			pack_text = _window.gadget(FIRST_PACKAGE + (idx<<1));
			pack_text.value(new_list[idx].display_text());
			if (new_list[idx].auto_action())
			{
				pack_button = _window.gadget(FIRST_PACKAGE + (idx<<1) + 1);
				pack_button.fade(true);
			}
		}
	}

	// Deleted from end of list so get last item to be used for resizing the area
	if (pack_text.null()) pack_text = _window.gadget(FIRST_PACKAGE + ((new_list.size()-1)<<1));

	for (unsigned int j = new_list.size(); j < _packages.size(); ++j)
	{
		_window.remove_gadget(FIRST_PACKAGE + (j<<1));
		_window.remove_gadget(FIRST_PACKAGE + (j<<1)+1);
	}

	tbx::DisplayField pack_area = _window.gadget(PACKAGE_AREA);
	tbx::BBox pack_area_bbox = pack_area.bounds();
	int min_y = pack_text.bounds().min.y - pack_gap;
	int pack_ymove = min_y - pack_area_bbox.min.y;
	pack_area_bbox.min.y = min_y;
	pack_area.bounds(pack_area_bbox);


	shift_below_pack_area(pack_ymove);

	_packages = new_list;
	update_download_totals();

	update_existing_components(new_comps);

	int comp_ymove = 0;

	if (new_list.size() != _components.size())
	{
		// Remove any left over components
		for (unsigned int comp_idx = new_comps.size(); comp_idx < _components.size(); ++comp_idx)
		{
			int first_id = FIRST_COMPONENT + (int(comp_idx) << COMP_SHIFT);
			for (int id_offset = 0; id_offset < COMP_SEPARATOR; ++id_offset)
			{
				_window.remove_gadget(first_id + id_offset);
			}
			if (comp_idx < _components.size() - 1)
			{
				_window.remove_gadget(first_id + COMP_SEPARATOR);
			}
		}

		tbx::DisplayField comp_area = _window.gadget(COMPONENT_AREA);
		tbx::BBox comp_bbox = comp_area.bounds();

		if (new_comps.empty())
		{
			comp_area.text(_no_component_text);
			min_y = comp_bbox.max.y - 40;
		} else
		{
			int last_sep_id = FIRST_COMPONENT + (int(new_comps.size()-1) << COMP_SHIFT) + COMP_SEPARATOR;
			min_y = _window.gadget(last_sep_id).bounds().min.y;
			// Take off last separator
			_window.remove_gadget(last_sep_id);
		}
		if (min_y != comp_bbox.min.y)
		{
			comp_ymove = min_y - comp_bbox.min.y;
			comp_bbox.min.y = min_y;
			comp_area.bounds(comp_bbox);
			for (int id = CANCEL_BUTTON; id <= APPLY_BUTTON; ++id)
			{
				tbx::Gadget g = _window.gadget(id);
				g.move_by(0, comp_ymove);
			}
		}
	}
	_components = new_comps;

	int ext_ychange = pack_ymove + comp_ymove;
	if (ext_ychange)
	{
		tbx::BBox extent = _window.extent();
		extent.min.y += ext_ychange;
		_window.extent(extent);
	}

	update_apply_button();
	update_recomendations();
}


/**
 * Build lists of packages and components to configure from
 * the current selection
 *
 * @param new_list new list of packages
 * @param new_comps new list of components
 */
void PackageConfigWindow::build_lists_from_selection(std::vector<PackageInfo> &new_list, std::vector<pkg::component> &new_comps)
{
	pkg::pkgbase * package_base = Packages::instance()->package_base ();
	pkg::status_table & seltable = package_base->selstat ();

	_download_count = 0;
	_download_size = 0;

	int auto_idx = -1;

	// Scan selected table for packages to configure
	for (pkg::status_table::const_iterator i = seltable.begin();
			i != seltable.end (); ++i)
	{
		const std::string &pkgname = i->first;
		const pkg::status &selstat = i->second;
		pkg::status curstat = package_base->curstat ()[pkgname];
		bool add_package = false;
		Action action;
		std::string version;

		if (pkg::unpack_req (curstat, selstat))
		{
			action = INSTALL;
			bool upgrade = (curstat.state() == pkg::status::state_installed);
			if (selstat.flag(pkg::status::flag_auto))
			{
				action = (upgrade) ? AUTO_UPGRADE : AUTO_INSTALL;
			}
			else if (upgrade) action = UPGRADE;

			pkg::pkgbase * package_base = Packages::instance()->package_base ();
			const pkg::binary_control & ctrl = package_base->control ()[pkgname];

			pkg::control::const_iterator f = ctrl.find ("Size");
			if (f != ctrl.end ())
			{
			   std::istringstream in (f->second);
			   int size;
			   in >> size;
			   _download_size += size;
			}

			_download_count++;
			add_package = true;

			version = selstat.version();
			if (version.empty()) version = ctrl.version();

			if (curstat.state() != pkg::status::state_installed)
			{
				// New package so add components
				std::string comps = ctrl.components();
				if (!comps.empty()) pkg::parse_component_list(comps.begin(), comps.end(), &new_comps);
			}
			//TODO: Do we need to somehow check if upgrade components have changed?
		} else if (pkg::remove_req(curstat, i->second))
		{
			action = REMOVE;
			if (curstat.flag(pkg::status::flag_auto)) action = AUTO_REMOVE;
			add_package = true;
			version = selstat.version();
			if (version.empty())
			{
				pkg::pkgbase * package_base = Packages::instance()->package_base ();
				const pkg::binary_control & ctrl = package_base->control ()[pkgname];
				version = ctrl.version();
			}
		}

		if (add_package)
		{
			// Ensure selected packages before auto-added packages
			if (action == AUTO_INSTALL || action == AUTO_REMOVE || action == AUTO_UPGRADE)
			{
				new_list.push_back(PackageInfo(pkgname, version, action));
				if (auto_idx == -1) auto_idx = new_list.size()-1;
			} else if (auto_idx == -1)
			{
				new_list.push_back(PackageInfo(pkgname, version, action));
			} else
			{
				new_list.insert(new_list.begin() + auto_idx, PackageInfo(pkgname, version, action));
				auto_idx++;
			}
		}
	}
}

/**
 * Update the totals for downloads
 */
void PackageConfigWindow::update_download_totals()
{
	tbx::DisplayField download_total(_window.gadget(DOWNLOAD_COUNT));
	tbx::DisplayField download_size(_window.gadget(DOWNLOAD_SIZE));
	download_total.text(tbx::to_string(_download_count));
	download_size.text(abbrev_size(_download_size));
}

/**
 * Update recommendations and suggestions count and enable view button
 */
void PackageConfigWindow::update_recomendations()
{
	std::vector< std::pair<std::string,std::string> > packages_to_check;
	std::vector< std::string > recs_found;

	for(std::vector<PackageInfo>::iterator i = _packages.begin(); i != _packages.end(); ++i)
	{
		if (i->action != REMOVE && i->action != AUTO_REMOVE)
		{
			packages_to_check.push_back(std::make_pair(i->name,i->version));
		}
	}

	Packages::instance()->get_recommendations(packages_to_check, recs_found, recs_found);
	tbx::DisplayField recs_count(_window.gadget(RECOMMEND_COUNT));
	tbx::ActionButton recs_browse(_window.gadget(RECOMMEND_BROWSE));

	recs_count.text(tbx::to_string(recs_found.size()));
	recs_browse.fade(recs_found.empty());
}

/**
 * Update apply button to reflect actions that will be carried out
 */
void PackageConfigWindow::update_apply_button()
{
	bool install = false, upgrade = false, remove = false;
	for (unsigned int j = 0; j < _packages.size();++j)
	{
		switch(_packages[j].action)
		{
		case INSTALL:
			install = true;
			break;
		case UPGRADE:
			upgrade = true;
			break;
		case REMOVE:
			remove = true;
			break;
		case AUTO_INSTALL:
		case AUTO_REMOVE:
		case AUTO_UPGRADE:
			// These do not affect the button text
			break;
		}
	}

	std::string new_text;
	if (install) new_text = _install_text;
	if (upgrade)
	{
		if (!new_text.empty()) new_text += apply_delim;
		new_text += _upgrade_text;
	}
	if (remove)
	{
		if (!new_text.empty()) new_text += apply_delim;
		new_text += _remove_text;
	}
	tbx::ActionButton apply_button = _window.gadget(APPLY_BUTTON);
	apply_button.text(new_text);
}

/**
 * Update UI for new component list by by copy existing items
 *
 * Does not add new items or delete any extras
 *
 * @param new_comps new component list
 */
void PackageConfigWindow::update_existing_components(std::vector<pkg::component> &new_comps)
{
	int size = (int)_components.size();

	if (size > (int)new_comps.size()) size = (int)new_comps.size();
	if (size > 0)
	{
		// Save any we may overwrite
		pkg::component current[size];
		bool current_path_set[size];
		for (int comp_no = 0; comp_no < size; ++comp_no)
		{
			if (new_comps[comp_no] != _components[comp_no])
			{
				current[comp_no] = pkg::component(_components[comp_no]);
				get_component(comp_no, current[comp_no], current_path_set[size]);
			}
		}
		// Fill in the new details
		for (int comp_no = 0; comp_no < size; ++comp_no)
		{
			if (new_comps[comp_no] != _components[comp_no])
			{
				int found_idx = find_component(new_comps[comp_no]);
				pkg::component item;
				bool path_set;

				if (found_idx == -1)
				{
					// New component
					item = pkg::component(new_comps[comp_no]);
					tbx::Path path(Packages::instance()->package_base()->paths()(item.name(), ""));
					item.path(path);
					path_set = false;
				} else if (found_idx < size)
				{
					// Component we saved in case it got overwritten
					item = current[found_idx];
					path_set = current_path_set[found_idx];
				} else
				{
					// Component from end of the old list
					item = pkg::component(new_comps[comp_no]);
					get_component(found_idx, item, path_set);
				}
				set_component(comp_no, item, path_set);
			}
		}
	}
}

/**
 * Shift all gadgets after the package area up or down by the given amount
 *
 * @param pack_ymove amount to move all the gadgets
 */
void PackageConfigWindow::shift_below_pack_area(int pack_ymove)
{
	for (int id = RECOMMEND_COUNT; id <= APPLY_BUTTON; ++id)
	{
		tbx::Gadget g = _window.gadget(id);
		g.move_by(0, pack_ymove);
	}

	if (!_components.empty())
	{
		for (unsigned int comp_idx = 0; comp_idx < _components.size(); ++comp_idx)
		{
			int first_id = FIRST_COMPONENT + (int(comp_idx) << COMP_SHIFT);
			for (int id_offset = 0; id_offset < COMP_SEPARATOR; ++id_offset)
			{
				tbx::Gadget g = _window.gadget(first_id + id_offset);
				g.move_by(0, pack_ymove);
			}
			if (comp_idx < _components.size() - 1)
			{
				tbx::Gadget sep = _window.gadget(first_id + COMP_SEPARATOR);
				sep.move_by(0, pack_ymove);
			}
		}
	}
}

/**
 * Find package in existing list
 *
 * @param name name to search for
 * @returns index package found at or -1 if not found
 */
int PackageConfigWindow::find_package(const std::string &name) const
{
	for (unsigned int i = 0; i < _packages.size(); ++i)
	{
		if (name == _packages[i].name) return (int)i;
	}
	return -1;
}

/**
 * Get details from the window for one component
 *
 * @param idx component index
 * @param item component item updated with values from the window
 * @param path_set set to true if the path has been set in the window
 */
void PackageConfigWindow::get_component(int idx, pkg::component &item, bool &path_set)
{
	int first_id = FIRST_COMPONENT + (idx << COMP_SHIFT);
	tbx::Button but = _window.gadget(first_id + COMP_PATH);
	tbx::Path path(but.value());
	tbx::DisplayField df = _window.gadget(first_id + COMP_LEAF);
	path.down(df.text());
	item.path( path );
	path_set = (but.foreground() != _default_path_colour);
	tbx::OptionButton ob = _window.gadget(first_id + COMP_LOOK_AT);
	item.flag(pkg::component::look_at,ob.on());
	ob = _window.gadget(first_id + COMP_RUN);
	item.flag(pkg::component::run, ob.on());
	ob = _window.gadget(first_id + COMP_ADD_TO_APPS);
	item.flag(pkg::component::add_to_apps,ob.on());

/*
	std::cout << "get comp " << idx << " "
			<< item.name() << " " << item.path() << " "
			<< item.look_at() << " " << item.run() << " " << item.add_to_apps()
			<< std::endl;
			*/
}

/**
 * Set component details form comp_update item
 *
 * @param idx index of item to set
 * @param item item with details
 * @param path_set the path has been set in this window
 */
void PackageConfigWindow::set_component(int idx, const pkg::component &item, bool path_set)
{
	int first_id = FIRST_COMPONENT + (idx << COMP_SHIFT);
	tbx::Button but = _window.gadget(first_id + COMP_PATH);
	tbx::Path path(item.path());
	but.value(path.parent());
	but.foreground(path_set ? _default_path_colour : _changed_path_colour);
	tbx::OptionButton ob = _window.gadget(first_id + COMP_LOOK_AT);
	ob.on(item.flag(pkg::component::look_at));
	ob = _window.gadget(first_id + COMP_RUN);
	ob.on(item.flag(pkg::component::run));
	ob = _window.gadget(first_id + COMP_ADD_TO_APPS);
	ob.on(item.flag(pkg::component::add_to_apps));
	tbx::DisplayField df = _window.gadget(first_id + COMP_LEAF);
	df.text(path.leaf_name());
}

/**
 * Copy component values from one set of gadgets to another
 *
 * @param from_idx index of values to copy from
 * @param to_idx index to copy value to
 */
void PackageConfigWindow::copy_component_values(int from_idx, int to_idx)
{
	pkg::component item(_components[from_idx]);
	bool path_set;
	get_component(from_idx, item, path_set);
	set_component(to_idx, item, path_set);
}

/**
 * Find component in existing list
 *
 * @param name name to search for
 * @returns index component found at or -1 if not found
 */
int PackageConfigWindow::find_component(const std::string &name) const
{
	for (unsigned int i = 0; i < _components.size(); ++i)
	{
		if (name == _components[i].name()) return (int)i;
	}
	return -1;
}

/**
 * Get display text for package in the window
 */
std::string PackageConfigWindow::PackageInfo::display_text() const
{
	std::string msg;
	switch(action)
	{
	case INSTALL:      msg = "Install"; break;
	case AUTO_INSTALL: msg = "AutoInstall"; break;
	case REMOVE:       msg = "Remove"; break;
	case AUTO_REMOVE:  msg = "AutoRemove"; break;
	case UPGRADE:      msg = "Upgrade"; break;
	case AUTO_UPGRADE: msg = "AutoUpgrade"; break;
	}
	msg += " ";
	msg += name;
	msg += " ";
	msg += version;
	msg += " ";
	std::string summary;

    pkg::pkgbase * package_base = Packages::instance()->package_base ();
    pkg::binary_control_table::key_type key(name, pkg::version(version));
    const pkg::binary_control &bctrl = package_base->control()[key];
	msg += bctrl.short_description();
	if (msg.size() > 79) msg.erase(79);
	return msg;
}

void PackageConfigWindow::show()
{
	_window.show();
}
