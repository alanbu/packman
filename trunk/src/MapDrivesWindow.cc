/*
 * MapDrivesWindow.cc
 *
 *  Created on: 13 Nov 2015
 *      Author: alanb
 */

#include "MapDrivesWindow.h"
#include "InstallListLoadWindow.h"
#include "tbx/application.h"
#include "tbx/lockextentposition.h"
#include "tbx/view/gadgetlistview.h"
#include "tbx/res/reswindow.h"
#include "tbx/displayfield.h"
#include "tbx/writablefield.h"
#include "tbx/actionbutton.h"
#include "tbx/path.h"
#include "tbx/messagewindow.h"

const int BadComponentId = 0x1000;

MapDrivesWindow::MapDrivesWindow(InstallListLoadWindow *ill_window, std::set<std::string> &bad_drives) :
  _ill_window(ill_window),
  _window("MapDrives"),
  _cancel_command(this, &MapDrivesWindow::cancel),
  _boot_command(this, &MapDrivesWindow::make_boot),
  _map_command(this, &MapDrivesWindow::map),
  _num_bad(bad_drives.size())
{
    // Add extra components if more than one drive
    if (_num_bad > 1)
    {
		// set to reposition the buttons at the bottom
		tbx::LockExtentPosition lock_pos(_window);
		lock_pos.add_gadgets(7,9,tbx::LockExtentPosition::bottom_to_bottom|tbx::LockExtentPosition::top_to_bottom);

		// Set up more drives if necessary
		tbx::view::GadgetListView view;
		tbx::res::ResWindow res_window = tbx::app()->resource("MapDrives");
		view.setup(_window, res_window, BadComponentId,BadComponentId+1,8, true);
		view.add(bad_drives.size() - 1);
		lock_pos.extent_changed();
    }

    int id = BadComponentId;
    for (auto drive : bad_drives)
    {
    	_drive_map.push_back(std::make_pair(drive, drive));
    	tbx::DisplayField old_drive = _window.gadget(id++);
    	tbx::WritableField new_drive = _window.gadget(id++);
    	old_drive.text(drive);
    	new_drive.text(drive);
    }

    tbx::ActionButton cancel_button = _window.gadget(7);
    tbx::ActionButton boot_button = _window.gadget(8);
    tbx::ActionButton map_button = _window.gadget(9);

    cancel_button.add_selected_command(&_cancel_command);
    boot_button.add_selected_command(&_boot_command);
    map_button.add_select_command(&_map_command);

	tbx::ShowFullSpec loc;
	loc.wimp_window = tbx::ShowFullSpec::WINDOW_SHOW_TOP;
	loc.visible_area.scroll().x = 0;
	loc.visible_area.scroll().y = 0;
	loc.visible_area.bounds() = _window.extent();
	loc.visible_area.bounds().move_to(200, 200);
	_window.show(loc);
}

MapDrivesWindow::~MapDrivesWindow()
{
	_window.delete_object();
}

/**
 * Close window and return to install list window
 */
void MapDrivesWindow::cancel()
{
	_ill_window->reshow();
	delete this;
}

/**
 * Map all the drives to the current boot drive
 */
void MapDrivesWindow::make_boot()
{
	std::vector<std::pair<std::string, std::string>> mapping;
	int id = BadComponentId;
	std::string boot_drive(tbx::Path::canonicalise("<Boot$Dir>.^"));
	auto dot_pos = boot_drive.find('.');
	if (dot_pos != std::string::npos) boot_drive.erase(dot_pos);
	for (int j = 0; j < _num_bad; j++)
	{
		tbx::DisplayField df = _window.gadget(id);
		std::string drive(df.text());
		mapping.push_back(std::make_pair(drive, boot_drive));
		id+=2;
	}
	_ill_window->remap_paths(mapping);
	delete this;
}

/**
 * Check new drives and apply mapping
 */
void MapDrivesWindow::map()
{
	std::vector<std::pair<std::string, std::string>> mapping;
	int id = BadComponentId;
	for (int j = 0; j < _num_bad; j++)
	{
		tbx::DisplayField df = _window.gadget(id++);
		std::string drive(df.text());
		tbx::WritableField wf = _window.gadget(id++);
		std::string new_drive(wf.text());
		mapping.push_back(std::make_pair(drive, new_drive));
		if (!_ill_window->check_drive(new_drive))
		{
			std::string msg("Invalid mapped drive ");
			msg += new_drive;
			msg += "\n\nPlease ensure you have typed it correctly and";
			msg += "\nit has been seen by the filer";
			tbx::show_message_as_menu(msg, "Invalid drive mapping", "error");

			return; // Report bad drives one at a time
		}
	}

	// All drives were correct
	_ill_window->remap_paths(mapping);
	delete this;
}
