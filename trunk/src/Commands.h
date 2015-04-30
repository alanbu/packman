/*********************************************************************
* Copyright 2009-2015 Alan Buckley
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
 * InstallCommand.h
 *
 *  Created on: 25-Mar-2009
 *      Author: alanb
 */

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include "tbx/command.h"

class MainWindow;
class IRWindow;
class UpgradeAllWindow;

namespace pkg
{
class binary_control;
}

/**
 * Interface to return the selected package for the
 * install/remove commands
 */
class ISelectedPackage
{
public:
	virtual ~ISelectedPackage() {}
	virtual const pkg::binary_control *selected_package() = 0;
};

/**
 * Command to install current item
 */
class InstallCommand : public tbx::Command
{
private:
	ISelectedPackage *_main;
public:
	enum {COMMAND_ID = 1}; // ID in resources

	InstallCommand(ISelectedPackage *main) : _main(main) {}
	virtual ~InstallCommand() {}

	virtual void execute();
};

/**
 * Command to remove current item
 */
class RemoveCommand : public tbx::Command
{
private:
	ISelectedPackage *_main;
public:
	enum {COMMAND_ID = 2}; // ID in resources

	RemoveCommand(ISelectedPackage *main) : _main(main) {}
	virtual ~RemoveCommand() {}

	virtual void execute();
};

/**
 * Command to commit the current install or remove
 */
class CommitCommand : public tbx::Command
{
public:
	virtual void execute();
};

/**
 * Command to update the list of packages
 */
class UpdateListCommand : public tbx::Command
{
public:
	enum {COMMAND_ID = 3}; // ID in resources

	virtual void execute();
};

/**
 * Show the sources window if it's not already shown
 */
class ShowSourcesWindowCommand : public tbx::Command
{
public:
	enum { COMMAND_ID = 4 };
	virtual void execute();
};

class ShowPathsWindowCommand : public tbx::Command
{
public:
	enum { COMMAND_ID = 5 };
	virtual void execute();
};

/**
 * Command ID for the Help command
 */
const int HELP_COMMAND_ID = 6;

/**
 * Command to upgrade all packages current item
 */
class UpgradeAllCommand : public tbx::Command
{
private:
	UpgradeAllWindow *_upgrade;
public:
	enum {COMMAND_ID = 7}; // ID in resources

	UpgradeAllCommand() : _upgrade(0) {}
	virtual ~UpgradeAllCommand() {}

	virtual void execute();
};

/**
 * Show the sources window if it's not already shown
 */
class ShowBackupWindowCommand : public tbx::Command
{
public:
	enum { COMMAND_ID = 8 };
	virtual void execute();
};

/**
 * Run the verify all installed packages command
 */
class VerifyAllCommand : public tbx::Command
{
public:
	virtual void execute();
};

/**
 * Menu option for verify all command that prompts to
 * run it first
 */
class VerifyAllAskCommand : public tbx::Command
{
	VerifyAllCommand _verify_all_command;
public:
	enum { COMMAND_ID = 9 };
	virtual void execute();
};


/**
 * Show the main window with the "What's New" filter selected
 */
class ShowWhatsNewCommand : public tbx::Command
{
public:
	virtual void execute();
};

/**
 * Show the log viewer window which shows the last log
 */
 class ShowLogViewerCommand : public tbx::Command
 {
 public:
       enum { COMMAND_ID = 10 };
       virtual void execute();
 };

 /**
  * Command to show info on the current item
  */
 class ShowInfoCommand : public tbx::Command
 {
 private:
 	MainWindow *_main;
 public:
 	enum {COMMAND_ID = 11}; // ID in resources

 	ShowInfoCommand(MainWindow *main) : _main(main) {}
 	virtual ~ShowInfoCommand() {}

 	virtual void execute();
 };

 /**
  * Command to show the choices window
  */
 class ShowChoicesWindowCommand : public tbx::Command
 {
 public:
	 enum {COMMAND_ID = 12}; // ID in resources
	 virtual ~ShowChoicesWindowCommand() {} // Stop compiler warning
	 virtual void execute();
 };

 /**
  * Command used from Main window to save the position
  */
const int SAVE_MAIN_WINDOW_POSITION_COMMAND = 13;

/**
 * Command to show copyright window
 */
class CopyrightCommand : public tbx::Command
{
private:
	ISelectedPackage *_main;
public:
	enum {COMMAND_ID = 14}; // ID in resources

	CopyrightCommand(ISelectedPackage *main) : _main(main) {}
	virtual ~CopyrightCommand() {}

	virtual void execute();
};

#endif /* COMMANDS_H_ */
