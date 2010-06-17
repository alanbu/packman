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
 * MainWindow.cc
 *
 *  Created on: 20-Mar-2009
 *      Author: alanb
 */

#include "MainWindow.h"
#include "Packages.h"
#include "SummaryWindow.h"
#include "PackageFilter.h"

#include "tbx/application.h"
#include "tbx/stringset.h"

#include <sstream>
#include "libpkg/pkgbase.h"


MainWindow::MainWindow() : _window("Main"), _view(_window),
   _status_sprite(this),
   _name_text(this),
   _summary_text(this),
   _status_renderer(&_status_sprite),
   _name_renderer(&_name_text),
   _summary_renderer(&_summary_text),
   _install(this),
   _remove(this),
   _store_menu_select(&_view)
{
	_view.add_column(&_status_renderer, 50);
	_view.add_column(&_name_renderer, 100);
	_view.add_column(&_summary_renderer, 400);
	_view.selection(&_selection);
	_view.margin(tbx::Margin(0,52,0,252));
	_view.auto_size(false);
	_view.menu_selects(true);

	_window.menu().add_has_been_hidden_listener(&_store_menu_select);

	_window.add_command(InstallCommand::COMMAND_ID, &_install);
	_window.add_command(RemoveCommand::COMMAND_ID, &_remove);

	_summary = new SummaryWindow(this, _window, &_selection);

	_filter = 0; // Start with all packages
	_search_filter = 0; // Search in all packages
	refresh();
	_window.show();

	tbx::Window install_tb = _window.itl_toolbar();
	_install_button = install_tb.gadget(0);
	_remove_button = install_tb.gadget(1);
	_filters_stringset = install_tb.gadget(3);

	_selection.add_listener(this);

	std::string sects = Packages::instance()->sections();
	std::string filter_set("All,Installed,Upgrades,Search Results");
	if (!sects.empty()) filter_set += "," + sects;
	_filters_stringset.available(filter_set);

	_filters_stringset.add_text_changed_listener(this);

	watch(Packages::instance()->package_base()->curstat());
	watch(Packages::instance()->package_base()->control());


   _window.client_handle(this);
}

MainWindow::~MainWindow()
{
    delete _summary;
    delete _filter;
    if (_filter != _search_filter) delete _search_filter;
}

/**
 * Static function to return the main window from its toolbox window
 */
MainWindow *MainWindow::from_window(tbx::Window window)
{
	return reinterpret_cast<MainWindow *>(window.client_handle());
}

/**
 * Refresh the main windows contents
 */
void MainWindow::refresh()
{
	_view.cleared();
	_shown_packages.clear();
	_store_menu_select.menu_selection = tbx::view::ItemView::NO_INDEX;

   pkg::pkgbase *package_base = Packages::instance()->package_base();
   const pkg::binary_control_table& ctrltab = package_base->control();
   std::string prev_pkgname;

   for (pkg::binary_control_table::const_iterator i=ctrltab.begin();
	 i !=ctrltab.end(); ++i)
   {
	  std::string pkgname=i->first.pkgname;
	  if (pkgname!=prev_pkgname)
	  {
		  // Don't use i->second for ctrl as it may not be the latest version
		  // instead look it up.
		  const pkg::binary_control &ctrl = ctrltab[pkgname];
		  prev_pkgname=pkgname;

		  if (_filter == 0 || _filter->ok_to_show(ctrl))
		  {
			  _shown_packages.push_back(&ctrl);
		  }
	  }
   }

   _view.inserted(0, _shown_packages.size());
   _view.size_column_to_width(1);
   _view.size_column_to_width(2);
}

// Sprites to use
tbx::UserSprite MainWindow::StatusSprite::_sprites[4];


MainWindow::StatusSprite::StatusSprite(MainWindow *me) : _me(me)
{
	if (!_sprites[0].is_valid())
	{
		// Load sprite when first instance of class is created
		tbx::SpriteArea *area = tbx::app()->sprite_area();
		_sprites[0] = area->get_sprite("status_i");
		_sprites[1] = area->get_sprite("status_u");
		_sprites[2] = area->get_sprite("status_ia");
		_sprites[3] = area->get_sprite("status_ua");
	}
};

tbx::Sprite *MainWindow::StatusSprite::value(unsigned int index) const
{
	std::string pkgname = _me->_shown_packages[index]->pkgname();
	bool auto_installed;
	int sprite_index = (int)(_me->install_state(_me->_shown_packages[index], &auto_installed)) - 1;
	if (auto_installed) sprite_index |= 2;

	return (sprite_index < 0) ? 0 : &_sprites[sprite_index];
}

/**
 * Get text to render status - Will change to using a sprite later
 */
/** Old code to get text for status
std::string MainWindow::StatusRenderer::text(unsigned int index) const
{
	std::string pkgname = _me->_shown_packages[index]->pkgname();
	std::string status;

    pkg::pkgbase *package_base = Packages::instance()->package_base();
	pkg::status_table::const_iterator sti = package_base->curstat().find(pkgname);
	if (sti == package_base->curstat().end()
		  || (*sti).second.state() != pkg::status::state_installed
		  )
	{
		  status = "";
	} else
	{
		  pkg::version inst_version((*sti).second.version());
		  pkg::version cur_version(_me->_shown_packages[index]->version());
		  if (inst_version >= cur_version)
			  status = "i";
		  else
			  status = "u";

		  if ((*sti).second.flag(pkg::status::flag_auto))
		  {
			  status += "a";
		  }
	}

	return status;
}
*/
/**
 * Return the currently selected package
 *
 * @returns currently selected package or 0 if no package is selected
 */
const pkg::binary_control *MainWindow::selected_package()
{
	unsigned int index;
	if (_view.selection()->empty())
	{
		index = _store_menu_select.menu_selection;
		if (index == tbx::view::ItemView::NO_INDEX)
			return 0;
	} else
	{
		index = _view.selection()->first();
	}

	return _shown_packages[index];
}

/**
 * Text in filter box has changed so update view.
 */
void MainWindow::text_changed(tbx::TextChangedEvent &event)
{
	std::string name(event.text());

	if (_filter != _search_filter) delete _filter;
	_filter = 0;

	if (name == "All")
	{
		// Do nothing _filter = 0 is all packages
	} else if (name == "Installed")
	{
		_filter = new InstalledFilter();
	} else if (name == "Upgrades")
	{
		_filter = new UpgradeFilter();
	} else if (name == "Search Results")
	{
		_filter = _search_filter;
	} else
	{
		// All others should be section filters
		_filter = new SectionFilter(name);
	}

	refresh();
}

/**
 * Watched table has changed so update display
 */
void MainWindow::handle_change(pkg::table& t)
{
    pkg::pkgbase *package_base = Packages::instance()->package_base();

	if (&t == &(package_base->curstat()))
	{
		// Refresh window status of package has changed
		_view.refresh();

		// Deselect and reselect to update toolbars
		unsigned int index;
		if (_view.selection()->empty())
		{
			index = _store_menu_select.menu_selection;
		} else
		{
			index = _view.selection()->first();
		}
		if (index != tbx::view::ItemView::NO_INDEX)
		{
			_view.selection()->clear();
			_view.selection()->select(index);
		}

	}
	else if (&t==&(package_base->control()))
	{
		// List of packages has changed
		refresh();
	}
}

/**
 * Search for text in name or description
 *
 * @param text text to search for
 * @param in current filter - restrict search to current filter
 */
void MainWindow::search(const std::string &text, bool in_current_filter)
{
	if (in_current_filter)
	{
		PackageFilter *search_filter = new SearchFilter(text, _filter);
		if (_search_filter != _filter) delete _search_filter;
		_search_filter = search_filter;
	} else
	{
		if (_search_filter != _filter) delete _search_filter;
		delete _filter;
		_search_filter = new SearchFilter(text, 0);
	}
	_filter = _search_filter;

	// Change string set to "Search Results" if necessary
	if (_filters_stringset.selected() != "Search Results")
	{
		// This will automatically update the display
		_filters_stringset.selected("Search Results");
	}
	refresh();
}

/**
 * Selection changed so update install/remove buttons.
 */
void MainWindow::selection_changed(const tbx::view::SelectionChangedEvent &event)
{
	if (!event.final()) return; // Only interested in last event in sequence

	bool fade_install;
	bool fade_remove;

	if (event.selected())
	{
		switch(install_state(_shown_packages[event.first()]))
		{
		case NOT_INSTALLED:
			fade_install = false;
			fade_remove = true;
			_install_button.text("Install");
			break;
		case INSTALLED:
			fade_install = true;
			fade_remove = false;
			break;
		case OLD_VERSION:
			_install_button.text("Upgrade");
			fade_install = false;
			fade_remove = false;
			break;
		}
	} else
	{
		fade_install = true;
		fade_remove = true;
	}

	_install_button.fade(fade_install);
	_remove_button.fade(fade_remove);
}

/**
 * Get the installed state of a package
 *
 * @param bctrl binary control record for package
 * @param auto_inst  pointer to variable to update with auto installed status
 *              or 0 if not interested.
 *
 * @returns InstallState with current state
 */
MainWindow::InstallState MainWindow::install_state(const pkg::binary_control *bctrl, bool *auto_inst /*= 0*/)
{
	std::string pkgname = bctrl->pkgname();
	InstallState state;
	if (auto_inst) *auto_inst = false;

    pkg::pkgbase *package_base = Packages::instance()->package_base();
	pkg::status_table::const_iterator sti = package_base->curstat().find(pkgname);
	if (sti == package_base->curstat().end()
		  || (*sti).second.state() != pkg::status::state_installed
		  )
	{
		state = NOT_INSTALLED;
	} else
	{
		  pkg::version inst_version((*sti).second.version());
		  pkg::version cur_version(bctrl->version());
		  if (inst_version >= cur_version)
			  state = INSTALLED;
		  else
			  state = OLD_VERSION;

		  if (auto_inst && (*sti).second.flag(pkg::status::flag_auto))
		  {
			  *auto_inst = true;
		  }
	}

	return state;
}
