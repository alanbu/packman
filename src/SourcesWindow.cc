/*********************************************************************
* This file is part of PackMan
*
* Copyright 2010-2020 AlanBuckley
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
 * SourcesWindow.cc
 *
 *  Created on: 10 Feb 2010
 *      Author: alanb
 */

#include "SourcesWindow.h"
#include "Packages.h"
#include "ErrorWindow.h"
#include "UpdateListWindow.h"
#include "tbx/application.h"
#include "tbx/deleteonhidden.h"
#include "libpkg/pkgbase.h"
#include "libpkg/source_table.h"

#include <kernel.h>


SourcesWindow::SourcesWindow() : _window("Sources"),
    _add(this, &SourcesWindow::add),
    _edit(this, &SourcesWindow::edit),
    _remove(this, &SourcesWindow::remove),
    _save(this, &SourcesWindow::save),
    _enable(this, &SourcesWindow::enable),
    _editor(0),
    _ignore_next_select(false)
{
	_sources = _window.gadget(1);
	_sources.add_selection_listener(this);
	_window.add_about_to_be_shown_listener(this);
	tbx::ActionButton add_button(_window.gadget(2));
	add_button.add_select_command(&_add);
	_edit_button = _window.gadget(7);
	_edit_button.add_select_command(&_edit);
	_remove_button = _window.gadget(3);
	_remove_button.add_select_command(&_remove);
	tbx::ActionButton save_button(_window.gadget(5));
	save_button.add_select_command(&_save);
	_enable_button = _window.gadget(8);
	_enable_button.add_select_command(&_enable);

    tbx::ActionButton known_button = _window.gadget(9);
    known_button.add_select_command(&_show_known_sources);

	_update_list = _window.gadget(6);

    // Dispose of object when it is hidden
	_window.add_has_been_hidden_listener(new tbx::DeleteClassOnHidden<SourcesWindow>(this));
	_window.add_has_been_hidden_listener(new tbx::DeleteObjectOnHidden());
}

SourcesWindow::~SourcesWindow()
{
	delete _editor;
}

/**
 * Load current list of sources as window is shown
 */
void SourcesWindow::about_to_be_shown(tbx::AboutToBeShownEvent &event)
{
	tbx::WimpSprite tick("tick");
	_sources_file.load();
	for (auto source : _sources_file.sources())
	{
		if (source.second)
		{
			_sources.add_item(source.first, tick);
		} else
		{
			_sources.add_item(source.first);
		}
	}
}

/**
 * Item selected on scroll list so update button or edit on double click
 */
void SourcesWindow::scrolllist_selection(const tbx::ScrollListSelectionEvent &event)
{
	// Need to ignore the reselect after the enable button as it
	// passes an incorrect index
	if (_ignore_next_select)
	{
		_ignore_next_select = false;
		return;
	}

	bool fade = (event.index() == -1);
	_edit_button.fade(fade);
	_remove_button.fade(fade);
	_enable_button.fade(fade);
	if (!fade) _enable_button.text(_sources_file.enabled(event.index()) ? "Disable" : "Enable");
	if (!fade && event.double_click()) edit();
}

/**
 * Get source for a specific index
 */
std::string SourcesWindow::source(int index)
{
	return _sources.item_text(index);
}

/**
 * Add/Edit source
 *
 * @param index of source to replace or -1 to add
 * @param new source
 */
void SourcesWindow::source(int index, const std::string &url)
{
	if (index == -1)
	{
		_sources.add_item(url, tbx::WimpSprite("tick"));
		_sources_file.add(url, true);
	}
	else
	{
		_sources.item_text(index, url);
		_sources_file.url(index, url);
	}
}

/**
 * Show window to add an item
 */
void SourcesWindow::add()
{
	if (!_editor) _editor = new SourceEdit(this);
	_editor->add_show();
}

/**
 * Show window to edit a window
 */
void SourcesWindow::edit()
{
	int index = _sources.first_selected();
	if (index >= 0)
	{
		if (!_editor) _editor = new SourceEdit(this);
		_editor->edit_show(index);
	}
}

/**
 * Remove source from window
 */
void SourcesWindow::remove()
{
	int index = _sources.first_selected();
	if (index >= 0)
	{
		_sources.delete_item(index);
		_sources_file.erase(index);
		_edit_button.fade(true);
		_remove_button.fade(true);
		_enable_button.fade(true);
	}
}

/**
 * Enable or disable the current selected source
 */
void SourcesWindow::enable()
{
	int index = _sources.first_selected();
	if (index >= 0)
	{
		_sources.deselect_item(index);
		_sources.delete_item(index);
		if(_sources_file.enabled(index))
		{
			_sources_file.enable(index, false);
			_sources.add_item(_sources_file.url(index), index);
        } else
		{
			_sources_file.enable(index, true);
			_sources.add_item(_sources_file.url(index), tbx::WimpSprite("tick"), index);
       	}
		_ignore_next_select = true;
		_sources.select_unique_item(index);
        _enable_button.text(_sources_file.enabled(index) ? "Disable" : "Enable");
    }
}

/**
 * Command to show known sources html page
 */
void SourcesWindow::ShowKnownSources::execute()
{
	tbx::app()->os_cli("Filer_Run <PackMan$Dir>.html.knownsources/htm");
}


/**
 * Save sources back to Sources file
 */
void SourcesWindow::save()
{
	try
	{
		_sources_file.save();
		bool update = _update_list.on();
		_window.hide();
		pkg::pkgbase *pkgbase = Packages::instance()->package_base();
		pkg::source_table &sources =pkgbase->sources();
		sources.update();
		if (update) new UpdateListWindow();
	} catch (std::exception &e)
	{
		new ErrorWindow(e.what(), "Sources save failed");
	}
}

/**
 * Class to edit source URL
 */
SourceEdit::SourceEdit(SourcesWindow *sources) :
		_sources(sources),
		_edit_index(-1),
		_window("SourceEdit"),
		_url(_window.gadget(1)),
		_ok(this, &SourceEdit::ok)
{
	tbx::ActionButton ok_button(_window.gadget(3));
	ok_button.add_select_command(&_ok);
}

SourceEdit::~SourceEdit()
{
	_window.delete_object();
}

/**
 * Show the window for adding a new source
 */
void SourceEdit::add_show()
{
	_edit_index = -1;
	_window.title("Add a new package source");
	_url.text("");
	_window.show();
}

/**
 * Show the window for editing an existing source
 */
void SourceEdit::edit_show(int index)
{
	std::string url = _sources->source(index);
	_edit_index = index;
	_window.title("Edit a package source");
	_url.text(url);
	_window.show();
}

/**
 * OK button click so update list of sources
 */
void SourceEdit::ok()
{
	std::string url = _url.text();
	if (!url.empty()) _sources->source(_edit_index, url);
}

