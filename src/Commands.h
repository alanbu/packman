/*********************************************************************
* This file is part of PackMan
*
* Copyright 2009-2020 AlanBuckley
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
 * InstallCommand.h
 *
 *  Created on: 25-Mar-2009
 *      Author: alanb
 */

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include "tbx/command.h"
#include "tbx/menu.h"

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

/**
 * Command to show FindFile window
 */
class FindFileCommand : public tbx::Command
{
public:
	enum {COMMAND_ID = 15}; // ID in resources
	FindFileCommand() {}
	virtual ~FindFileCommand() {}
	virtual void execute();
};

/**
 * Command to show the environment window
 */
class ShowEnvironmentCommand : public tbx::Command
{
public:
	enum {COMMAND_ID = 16}; // ID in resources
	ShowEnvironmentCommand() {}
	virtual ~ShowEnvironmentCommand() {}
	virtual void execute();
};

/**
 * Command to show list of installed files
 */
class ShowFileListCommand : public tbx::Command
{
private:
	ISelectedPackage *_main;
public:
	enum {COMMAND_ID = 17}; // ID in resources
	ShowFileListCommand(ISelectedPackage *main) : _main(main) {}
	virtual ~ShowFileListCommand() {}
	virtual void execute();
};

#endif /* COMMANDS_H_ */
