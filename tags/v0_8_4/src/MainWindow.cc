/*********************************************************************
* Copyright 2009-2014 Alan Buckley
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
#include "Choices.h"

#include "tbx/application.h"
#include "tbx/stringset.h"
#include "tbx/deleteonhidden.h"
#include "tbx/objectdelete.h"

#include <sstream>
#include "libpkg/pkgbase.h"

#ifdef MAGIC_CHECK
const unsigned int ALLOC_MAGIC = 0xaaaaaaaa;
const unsigned int DEALLOC_MAGIC = 0x55555555;
#endif

MainWindow::MainWindow() : _window("Main"), _view(_window),
   _status_sprite(this),
   _name_text(this),
   _summary_text(this),
   _status_renderer(&_status_sprite),
   _name_renderer(&_name_text),
   _summary_renderer(&_summary_text),
   _install(this),
   _remove(this),
   _show_info(this),
   _save_pos(this, &MainWindow::save_position),
   _store_menu_select(&_view),
   _show_info_on_dblclick(this)
{
#ifdef MAGIC_CHECK
	_magic = ALLOC_MAGIC;
#endif
	_view.add_column(&_status_renderer, 50);
	_view.add_column(&_name_renderer, 100);
	_view.add_column(&_summary_renderer, 400);
	_view.selection(&_selection);
	_view.margin(tbx::Margin(0,68,0,254));
	_view.auto_size(false);
	_view.menu_selects(true);
	_view.add_click_listener(&_show_info_on_dblclick);

	_window.menu().add_has_been_hidden_listener(&_store_menu_select);

	_window.add_command(InstallCommand::COMMAND_ID, &_install);
	_window.add_command(RemoveCommand::COMMAND_ID, &_remove);
	_window.add_command(ShowInfoCommand::COMMAND_ID, &_show_info);
	_window.add_command(SAVE_MAIN_WINDOW_POSITION_COMMAND, &_save_pos);

	_summary = new SummaryWindow(this, _window, &_selection);

	_status_filter = 0; // Start with all packages
	_section_filter = 0;
	_search_filter = 0; // Search in all packages
	refresh();

	tbx::ShowFullSpec show_spec;
	show_spec.visible_area.bounds() = choices().main_window_pos();
	show_spec.visible_area.scroll() = tbx::Point(0,0);
	show_spec.wimp_window = tbx::ShowFullSpec::WINDOW_SHOW_TOP;

	_window.show(show_spec);

	tbx::Window install_tb = _window.itl_toolbar();
	_install_button = install_tb.gadget(0);
	_remove_button = install_tb.gadget(1);
	_status_filter_stringset = install_tb.gadget(3);
	_section_filter_stringset = install_tb.gadget(10);
	_apps_button = install_tb.gadget(7);
	_info_button = install_tb.gadget(8);

	_selection.add_listener(this);

	std::string sects = Packages::instance()->sections();
	std::string section_set("All,Search Results");
	if (!sects.empty()) section_set += "," + sects;
	_section_filter_stringset.available(section_set);

	_status_filter_stringset.add_text_changed_listener(this);
	_section_filter_stringset.add_text_changed_listener(this);

	watch(Packages::instance()->package_base()->curstat());
	watch(Packages::instance()->package_base()->control());


   _window.client_handle(this);

   // Set up so hide window deletes the object class and then this class
   _window.add_has_been_hidden_listener(new tbx::DeleteObjectOnHidden());
   _window.add_object_deleted_listener(new tbx::ObjectDeleteClass<MainWindow>(this));

	if (choices().small_summary_bar())
	{
		_summary->on_toggle_size();
	}
}

MainWindow::~MainWindow()
{
	unwatch(Packages::instance()->package_base()->curstat());
	unwatch(Packages::instance()->package_base()->control());

    delete _summary;
    if (_section_filter != _search_filter) delete _section_filter;
    delete _status_filter;
    delete _search_filter;
#ifdef MAGIC_CHECK
	_magic = DEALLOC_MAGIC;
#endif
}

/**
 * Static function to return the main window from its toolbox window
 */
MainWindow *MainWindow::from_window(tbx::Window window)
{
	if (window.null()) throw std::invalid_argument("Attempting to get MainWindow from null toolbox handle");
	MainWindow *mw = reinterpret_cast<MainWindow *>(window.client_handle());
	if (mw == 0) throw std::invalid_argument("MainWindow has 0 client handle");
#ifdef MAGIC_CHECK
	if (mw->_magic != ALLOC_MAGIC)
	{
		if (mw->_magic == DEALLOC_MAGIC) throw std::invalid_argument("Attempting to use a deleted MainWindow");
		throw std::invalid_argument("Corrupted magic number in MainWindow");
	}
#endif
	return mw;
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
   const std::vector<std::string> &package_list = Packages::instance()->package_list();
   const pkg::binary_control_table& ctrltab = package_base->control();

   for (std::vector<std::string>::const_iterator i = package_list.begin();
	 i !=package_list.end(); ++i)
   {
	  std::string pkgname=*i;
	  const pkg::binary_control &ctrl = ctrltab[pkgname];

	  if ((_status_filter == 0 || _status_filter->ok_to_show(ctrl))
			  && (_section_filter == 0 || _section_filter->ok_to_show(ctrl))
			  )
	  {
		  _shown_packages.push_back(&ctrl);
	  }
   }

   if (!_shown_packages.empty())
   {
	   _view.inserted(0, _shown_packages.size());
	   _view.size_column_to_width(1);
	   _view.size_column_to_width(2);
   }
}

// Sprites to use
tbx::UserSprite MainWindow::StatusSprite::_sprites[8];


MainWindow::StatusSprite::StatusSprite(MainWindow *me) : _me(me)
{
	if (!_sprites[0].is_valid())
	{
		// Load sprite when first instance of class is created
		tbx::SpriteArea *area = tbx::app()->sprite_area();
		_sprites[0] = area->get_sprite("status_p");
		_sprites[1] = area->get_sprite("status_i");
		_sprites[2] = area->get_sprite("status_u");
		_sprites[4] = area->get_sprite("status_pa");
		_sprites[5] = area->get_sprite("status_ia");
		_sprites[6] = area->get_sprite("status_ua");
	}
};

tbx::Sprite *MainWindow::StatusSprite::value(unsigned int index) const
{
	std::string pkgname = _me->_shown_packages[index]->pkgname();
	bool auto_installed;
	int sprite_index = (int)(_me->install_state(_me->_shown_packages[index], &auto_installed)) - 1;
	if (auto_installed) sprite_index |= 4;

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

	return _shown_packages.at(index);
}

/**
 * Text in filter box has changed so update view.
 */
void MainWindow::text_changed(tbx::TextChangedEvent &event)
{
	std::string name(event.text());
	if (event.id_block().self_component() == _section_filter_stringset)
	{
		section_filter_changed(name);
	} else
	{
		status_filter_changed(name);
	}
}

/**
 * Update the display when the section filter has changed
 *
 * @param new name of filter
 */
void MainWindow::section_filter_changed(const std::string &name)
{
	if (_section_filter != _search_filter) delete _section_filter;
	_section_filter = 0;

	if (name == "All")
	{
		// Do nothing _filter = 0 is all packages
	} else if (name == "Search Results")
	{
		_section_filter = _search_filter;
	} else
	{
		// All others should be section filters
		_section_filter = new SectionFilter(name);
	}

	refresh();
}

/**
 * Update the display when the status filter has changed
 *
 * @param new name of filter
 */
void MainWindow::status_filter_changed(const std::string &name)
{
	if (_status_filter != _search_filter) delete _status_filter;
	_status_filter = 0;

	if (name == "All")
	{
		// Do nothing _filter = 0 is all packages
	} else if (name == "Installed")
	{
		_status_filter = new InstalledFilter();
	} else if (name == "Upgrades")
	{
		_status_filter = new UpgradeFilter();
	} else if (name == "What's New")
	{
		_status_filter = new WhatsNewFilter();
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
			update_toolbar(index);
			_summary->set_selection_text(false);
		}
	}
	else if (&t==&(package_base->control()))
	{
		// List of packages has changed
		Packages::instance()->reset_package_list();
		refresh();
	}
}

/**
 * Search for text in name or description
 *
 * @param text text to search for
 * @param in current filter - restrict search to current filter
 */
void MainWindow::search(const std::string &text, bool in_current_status, bool in_current_section)
{
	if (_section_filter == _search_filter) _section_filter = 0;
	delete _search_filter;
	_search_filter = 0;
	if (in_current_section)
	{
		_search_filter = new SearchFilter(text, _section_filter);
	} else
	{
		_search_filter = new SearchFilter(text, 0);
	}
	_section_filter = _search_filter;

	// Change string set to "Search Results" if necessary
	if (_section_filter_stringset.selected() != "Search Results")
	{
		// This will automatically update the display
		_section_filter_stringset.selected("Search Results");
	}
	if (!in_current_status)
	{
		if (_status_filter != 0)
		{
			delete _status_filter;
			_status_filter = 0;
			_status_filter_stringset.selected("All");
		}
	}
	refresh();
}

/**
 * Set the section filter for the window
 *
 * @param name name of the new filter
 */
void MainWindow::set_section_filter(const char *name)
{
	_section_filter_stringset.selected(name);
	section_filter_changed(name);
}

/**
 * Set the status filter for the window
 *
 * @param name name of the new filter
 */
void MainWindow::set_status_filter(const char *name)
{
	_status_filter_stringset.selected(name);
	status_filter_changed(name);
}


/**
 * Selection changed so update install/remove buttons.
 */
void MainWindow::selection_changed(const tbx::view::SelectionChangedEvent &event)
{
	if (!event.final()) return; // Only interested in last event in sequence

	if (event.selected())
	{
		update_toolbar(event.first());
	} else
	{
		_info_button.fade(true);
		_install_button.fade(true);
		_remove_button.fade(true);
		_apps_button.fade(true);
	}
}

/**
 * Update toolbar to fade/unfade buttons for given index
 *
 * @param index index of item to update toolbar for
 */
void MainWindow::update_toolbar(int index)
{
	bool fade_install = true;
	bool fade_remove = true;

	switch(install_state(_shown_packages[index]))
	{
	case NOT_INSTALLED:
		fade_install = false;
		fade_remove = true;
		_install_button.on(false);
		break;
	case PART_INSTALLED:
		fade_install = false;
		fade_remove = false;
		_install_button.on(false);
		break;
	case INSTALLED:
		fade_install = true;
		fade_remove = false;
		break;
	case OLD_VERSION:
		_install_button.on(true);
		fade_install = false;
		fade_remove = false;
		break;
	}
	_info_button.fade(false);
	_install_button.fade(fade_install);
	_remove_button.fade(fade_remove);
	_apps_button.fade(fade_remove);
}

/**
 * Get the installed state of a package
 *
 * @param bctrl binary control record for package
 * @param auto_inst  pointer to variable to update with auto installed status
 *              or 0 if not interested.
 *              For PART_INSTALLED packages this flag is used to indicate the files were unpacked
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
	if (sti == package_base->curstat().end())
	{
		state = NOT_INSTALLED;
	} else if ((*sti).second.state() != pkg::status::state_installed )
	{
		if ( (*sti).second.state() <= pkg::status::state_removed )
		{
			state = NOT_INSTALLED;
			if (auto_inst) *auto_inst = ( (*sti).second.state() >= pkg::status::state_unpacked );
		} else
		{
			state = PART_INSTALLED;
		}
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

/**
 * Summary window size has been changed
 *
 * @param by amount of changes
 */
void MainWindow::summary_size_changed(int by)
{
	tbx::Margin m = _view.margin();
	m.bottom += by;
	_view.margin(m);
}

/**
 * Save current position of window so it will be re-opened here later
 */
void MainWindow::save_position()
{
	tbx::WindowState state;
	_window.get_state(state);
	choices().main_window_pos(state.visible_area().bounds());
	choices().small_summary_bar(_summary->is_small_size());
	if (choices().modified()) choices().save();
}

/**
 * Show the information window if an item is double clicked
 */
void MainWindow::ShowInfoOnDblClick::itemview_clicked(const tbx::view::ItemViewClickEvent &event)
{
	if (event.item_hit() && event.click_event().is_select_double())
	{
		_main->_show_info.execute();
	}
}