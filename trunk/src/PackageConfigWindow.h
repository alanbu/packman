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
 * PackageConfigWindow.h
 *
 *  Created on: 18 Apr 2013
 *      Author: alanb
 */

#ifndef PackageConfigWindow_H_
#define PackageConfigWindow_H_

#include "Packages.h"

#include "tbx/window.h"
#include "tbx/scrolllist.h"
#include "tbx/displayfield.h"
#include "tbx/draggable.h"
#include "tbx/button.h"
#include "tbx/buttonselectedlistener.h"
#include "tbx/command.h"
#include "tbx/colour.h"
#include "PathChooser.h"
#include "libpkg/component.h"

#include <vector>
#include <string>

class PackageConfigWindow : tbx::ButtonSelectedListener
{
	tbx::Window _window;

	PathChooser _path_chooser;
	friend class PathChooser;

	tbx::CommandMethod<PackageConfigWindow> _apply_command;
	tbx::CommandMethod<PackageConfigWindow> _cancel_command;

	enum Action {INSTALL, UPGRADE, REMOVE, AUTO_INSTALL, AUTO_UPGRADE, AUTO_REMOVE};
	struct PackageInfo
	{
		PackageInfo() : action(INSTALL) {}
		PackageInfo(const std::string &n, const std::string &v, Action a) : name(n), version(v), action(a) {}
		bool operator==(const PackageInfo &other) const {return name == other.name && version == other.version && action == other.action;}
		bool operator!=(const PackageInfo &other) const {return name != other.name || version != other.version || action != other.action;}

		std::string display_text() const;
		bool auto_action() const {return (action > REMOVE);}

		std::string name;
		std::string version;
		Action action;
	};
	std::vector<PackageInfo> _packages;
	std::vector<pkg::component> _components;
	unsigned int _download_count;
	unsigned long long _download_size;

	int _row_height;
	int _top_margin;

	// Information read from the window
	std::string _no_component_text;
	std::string _install_text, _upgrade_text, _remove_text;
	tbx::WimpColour _default_path_colour;
	tbx::WimpColour _changed_path_colour;

	static PackageConfigWindow *_instance;
	static int _show_x;
	static int _show_y;

	enum GadgetID {PACKAGE_TEXT, PACKAGE_AREA,
		RECOMMEND_COUNT, RECOMMEND_TEXT, RECOMMEND_BROWSE,
		DOWNLOAD_COUNT, DOWNLOAD_TEXT, DOWNLOAD_SIZE,
		COMPONENT_TEXT, COMPONENT_AREA,
		CANCEL_BUTTON, APPLY_BUTTON,
		FIRST_PACKAGE = 0x1000,
		FIRST_COMPONENT = 0x8000
	};

	enum CompOffsetID {COMP_PATH, COMP_LEAF, COMP_DRAGGABLE,
		BOOT_OPTIONS_LABEL,
		COMP_LOOK_AT, COMP_RUN, COMP_ADD_TO_APPS, COMP_SEPARATOR,
		COMP_COUNT // Number of component ids
	};

	enum { COMP_SHIFT = 3};

public:
	PackageConfigWindow();
	virtual ~PackageConfigWindow();

	void package_added();
	void package_removed();

	static void update();
	static void bring_to_top();
	void show();

private:
	void apply();
	void cancel();

	virtual void button_selected(tbx::ButtonSelectedEvent &event);
	void remove_package_from_list(int package_idx);

	void shift_below_pack_area(int pack_ymove);

	void build_lists_from_selection(std::vector<PackageInfo> &new_list, std::vector<pkg::component> &new_comps);

	int find_package(const std::string &name) const;

	void update_existing_components(std::vector<pkg::component> &new_comps);
	void update_download_totals();
	void update_apply_button();

	void get_component(int idx, pkg::component &item, bool &path_set);
	void set_component(int idx, const pkg::component &item, bool path_set);
	void copy_component_values(int from_idx, int to_idx);
	int find_component(const std::string &name) const;

	// Helpers for PathChooser
	tbx::Draggable draggable_gadget(int idx) {return _window.gadget(FIRST_COMPONENT + (idx << COMP_SHIFT) + COMP_DRAGGABLE);}
	tbx::Button path_gadget(int idx) {return _window.gadget(FIRST_COMPONENT + (idx << COMP_SHIFT) + COMP_PATH);}
	tbx::DisplayField leaf_gadget(int idx) {return _window.gadget(FIRST_COMPONENT + (idx << COMP_SHIFT)+COMP_LEAF);}
	tbx::Button path_gadget(tbx::Draggable draggable) {return _window.gadget(draggable.id() - COMP_DRAGGABLE + COMP_PATH);}
	tbx::DisplayField leaf_gadget(tbx::Draggable draggable) {return _window.gadget(draggable.id() - COMP_DRAGGABLE + COMP_LEAF);}
};

#endif /* PackageConfigWindow_H_ */
