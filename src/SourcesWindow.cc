/*********************************************************************
* Copyright 2010 Alan Buckley
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
#include "tbx/path.h"
#include "tbx/deleteonhidden.h"
#include "libpkg/pkgbase.h"
#include "libpkg/source_table.h"
#include <fstream>

SourcesWindow::SourcesWindow() : _window("Sources"),
    _add(this, &SourcesWindow::add),
    _edit(this, &SourcesWindow::edit),
    _remove(this, &SourcesWindow::remove),
    _save(this, &SourcesWindow::save),
    _enable(this, &SourcesWindow::enable),
    _editor(0)
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

	//tbx::ActionButton enable_button = _window.gadget(??);
	// enable_button.add_select_command(&_enable);

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
	tbx::UserSprite tick = tbx::app()->sprite_area()->get_sprite("status_i");
	printf("Sprite ok %d\n", tick.is_valid());
	// Read sources from file so we also get the disabled sources
	std::ifstream in("Choices:PackMan.Sources");
	while (in&&!in.eof())
	{
		// Read line from input stream.
		std::string line;
		std::getline(in,line);

		printf("Got line %s\n", line.c_str());
		// Strip comments and trailing spaces.
		std::string::size_type n=line.find('#');
		if (n!=std::string::npos)
		{
			n++;
			while(n < line.size() && std::isspace(line[n])) line.erase(n);
			if (n < line.size() && line.compare(n,3,"pkg") != 0) n = 0;
			else n = line.find('#'); // Strip second set of comments
		}
		if (n==std::string::npos) n=line.size();

		while (n&&std::isspace(line[n-1])) --n;
		line.resize(n);

		// Extract source type and source path.
		std::string::size_type i=0;
		while ((i!=line.length())&&!std::isspace(line[i])) ++i;
		std::string srctype(line,0,i);
		while ((i!=line.length())&&std::isspace(line[i])) ++i;
		std::string srcpath(line,i,std::string::npos);

		// Ignore line if source type not recognised.
		if (srctype=="pkg")
		{
printf("Adding enabled url %s\n",srcpath.c_str());
			_sources.add_item(srcpath/*, tick*/);
			_source_info.push_back(std::make_pair(srcpath, true));
		} else if (srctype=="#pkg")
		{
			_sources.add_item(srcpath);
			_source_info.push_back(std::make_pair(srcpath, false));
		}

		// Check for end of file.
		in.peek();
	}
}

/**
 * Item selected on scroll list so update button or edit on double click
 */
void SourcesWindow::scrolllist_selection(const tbx::ScrollListSelectionEvent &event)
{
	bool fade = (event.index() == -1);
	_edit_button.fade(fade);
	_remove_button.fade(fade);
	//_enable_button.fade(fade);
	// if (!fade) _enable_button_text(_source_info[event.index()].second ? "Disable" : "Enable";
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
		_sources.add_item(url);
		_source_info.push_back(std::make_pair(url, true));
	}
	else
	{
		_sources.item_text(index, url);
		_source_info[index].first = url;
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
		_source_info.erase(_source_info.begin() + index);
		_edit_button.fade(true);
		_remove_button.fade(true);
		//_enable_button.fade(true);
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
		_sources.delete_item(index);
		if(_source_info[index].second)
		{
			_source_info[index].second = false;
			_sources.add_item(_source_info[index].first, index);
		} else
		{
			_source_info[index].second = true;
			_sources.add_item(_source_info[index].first, tbx::app()->sprite_area()->get_sprite("status_i"), index);
		}
	}
}

/**
 * Save sources back to Sources file
 */
void SourcesWindow::save()
{
	tbx::Path choices_dir("<Choices$Write>.PackMan");
	try
	{
	   choices_dir.create_directory();
	} catch(...)
	{
		new ErrorWindow("Unable to create ", choices_dir, "Sources save failure");
		return;
	}
	{
		tbx::Path sources_path = choices_dir.child("Sources");

		std::ofstream sfile(sources_path);
		if (sfile)
		{
			sfile << "# Sources written by PackMan" << std::endl << std::endl;
			int num_entries = _source_info.size();
			for (int i = 0; i < num_entries; i++)
			{
				if (!_source_info[i].second) sfile << "#";
				sfile << "pkg " << _source_info[i].first << std::endl;
			}
			sfile.close();
		}

		if (sfile)
		{
			bool update = _update_list.on();
			_window.hide();
			pkg::pkgbase *pkgbase = Packages::instance()->package_base();
			pkg::source_table &sources =pkgbase->sources();
			sources.update();
			if (update) new UpdateListWindow();
		}
		else
		{
			new ErrorWindow("Unable to write sources file ",sources_path, "Sources save failure");
		}
	}
}

/**
 * Class to edit source URL
 */
SourceEdit::SourceEdit(SourcesWindow *sources) :
		_sources(sources),
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

