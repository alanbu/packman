/*********************************************************************
* Copyright 2009-2012 Alan Buckley
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

#include "CreateStub.h"
#include "BackupAndRun.h"

#include "tbx/messagewindow.h"
#include "tbx/questionwindow.h"

#include <fstream>

/** Create obey file to link from stub to target.
 * Take no action if the target file does not exist.
 *
 * @param source_path path to location of the file to link to
 * @param path the location to create the link
 * @param leaf the item to link
 */
inline void create_file_stub(const tbx::Path &source_path, const tbx::Path &path, std::string leaf)
{
	tbx::Path src(source_path, leaf);
	tbx::Path dst(path, leaf);
	if (src.exists())
	{
		std::string contents("/");
		contents += src.name();
		contents += " %*0\n";
		dst.save_file(contents.c_str(), contents.size(), 0xfeb);
	}
}

/**
 * Create an application directory stub that puts obey
 * files in it to invoke the files in the original
 * application.
 *
 * @param source_path path to the application to link to
 * @param target_path path for the application stub
 */
inline void create_app_stub(const tbx::Path &source_path, const tbx::Path &target_path)
{
	if (target_path.create_directory())
	{
		create_file_stub(source_path, target_path, "!Boot");
		create_file_stub(source_path, target_path, "!Run");
		create_file_stub(source_path, target_path, "!Help");
	}
}


/**
 * Create an application directory stub link that creates
 * a LinkFS link to the application.
 *
 * @param source_path path to the application to link to
 * @param target_path path for the application link
 */
inline void create_app_link(const tbx::Path &source_path, tbx::Path &target_path)
{
	std::ofstream out(target_path);
	out << source_path.name() << '.' << char(0);
	out.close();
	target_path.file_type(0xfc0);
}


/**
 * Command to create a stub
 */
class CreateStubCommand : public tbx::Command
{
	tbx::Path _source;
	tbx::Path _target;
public:
	CreateStubCommand(const tbx::Path &source, const tbx::Path &target) :
		_source(source), _target(target)
	{
	}

	void execute()
	{
		create_app_stub(_source, _target);
	}
};

/**
 * Command to backup the target and then create a stub
 */
class BackupAndStubCommand : public tbx::Command
{
	tbx::Path _source;
	tbx::Path _target;
public:
	BackupAndStubCommand(const tbx::Path &source, const tbx::Path &target) :
		_source(source), _target(target)
	{
	}

	/**
	 * Start the window that does the backup and stub
	 */
	void execute()
	{
		BackupAndRun *bar = new BackupAndRun("Create stub after backup", new CreateStubCommand(_source, _target));
		bar->add(_target);
		bar->start();
	};
};

/**
 * Command to create a link
 */
class CreateLinkCommand : public tbx::Command
{
	tbx::Path _source;
	tbx::Path _target;
public:
	CreateLinkCommand(const tbx::Path &source, const tbx::Path &target) :
		_source(source), _target(target)
	{
	}

	void execute()
	{
		create_app_link(_source, _target);
	}
};

/**
 * Command to backup the target and then create a link
 */
class BackupAndLinkCommand : public tbx::Command
{
	tbx::Path _source;
	tbx::Path _target;
public:
	BackupAndLinkCommand(const tbx::Path &source, const tbx::Path &target) :
		_source(source), _target(target)
	{
	}

	/**
	 * Start the window that does the backup and stub
	 */
	void execute()
	{
		BackupAndRun *bar = new BackupAndRun("Create link after backup", new CreateLinkCommand(_source, _target));
		bar->add(_target);
		bar->start();
	};
};


/**
 * Create an application stub, with prompt and backup option if the
 * target already exists.
 *
 * @param source application to create stub to
 * @param target location of application stub
 */
void create_application_stub(const tbx::Path &source, const std::string &target)
{
	tbx::Path dst(target);
	if (dst.canonical_equals(source))
	{
		tbx::show_message("You can not create a stub on top of the application!", "PackMan", "error");
	} else if (dst.exists())
	{
		BackupAndStubCommand *yes_command = new BackupAndStubCommand(source, dst);
		tbx::show_question_as_menu("An application/stub or link already exists\n"
				           "at this location, would you like to back it\n"
				           "up and replace it?", "PackMan create stub",
				           yes_command,0,true);
	} else
	{
		create_app_stub(source, dst);
	}
}

/**
 * Create an application link, with prompt and backup option if the
 * target already exists.
 *
 * @param source application to create link to
 * @param target location of application link
 */
void create_application_link(const tbx::Path &source, const std::string &target)
{
	tbx::Path dst(target);
	if (dst.canonical_equals(source))
	{
		tbx::show_message("You can not create a link on top of the application!", "PackMan", "error");
	} else if (dst.exists())
	{
		BackupAndLinkCommand *yes_command = new BackupAndLinkCommand(source, dst);
		tbx::show_question_as_menu("An application/stub or link already exists\n"
				           "at this location, would you like to back it\n"
				           "up and replace it?", "PackMan create link",
				           yes_command,0,true);
	} else
	{
		create_app_link(source, dst);
	}
}
