

#include "look_at.h"

#include "fileutils.h"

#include <fstream>
#include <stdexcept>
#include <cstring>

#include <iostream>

/*
 * Notes on Boot.Desktop look at section
 *
 * Section starts
 * |Start RISCOS BootBoot 0.01 Boot
 * and ends
 * |End
 *
 * If it's not there is should be inserted after the section
 * |Start RISCOS !Boot 0.26 Auto tasks
 *
 * Entries should be added with the path relative to boot if possible
 *
 * Format of an entry is
 * IfThere HostFS:$.Dev.!GCC Then Filer_Boot HostFS:$.Dev.!GCC
 *
 * or if it was on the boot drive
 * IfThere Boot:^.Apps.Desktop.!Fred Then Filer_Boot Boot:^.Apps.Desktop.!Fred
 *
 */

/**
 * Construct look_at object by setting the correct path and reading
 * the Boot.Desktop file
 */
look_at::look_at() :
   _path_name("<Choices$Write>.Boot.Desktop"),
   _file_contents(0),
   _section(0),
   _end_section(0),
   _modified(false)
{
	rollback();
}

look_at::~look_at()
{
	delete [] _file_contents;
}

/**
 * Discard any changes and reload the file
 *
 * @throws std::runtime_error if it fails to open the file
 */
void look_at::rollback()
{
	delete [] _file_contents;
	_file_contents = 0;
	_section = 0;
	_modified = false;

	std::ifstream desktop_stream(_path_name.c_str());
	if (!desktop_stream) throw std::runtime_error("Unable to open Boot Desktop file");
	int length;
	desktop_stream.seekg(0, std::ios::end);
	length = desktop_stream.tellg(); // Get length of file
	if (length == 0) throw std::runtime_error("Boot Desktop file is empty");
	desktop_stream.seekg(0, std::ios::beg);
	_file_contents = new char[length+1];
	desktop_stream.read(_file_contents, length);
	_file_contents[length] = 0; // Zero terminate

	parse_section();
}

/**
 * Commit any changes to the file
 *
 * Does nothing if no modifications have been made
 *
 * @throws std::ios_base::failure if commit fails to write the file
 */
void look_at::commit()
{
	if (!_modified) return;

	if (_modified)
	{
		std::ofstream file;
		file.exceptions ( std::ofstream::failbit | std::ofstream::badbit );
		file.open(_path_name.c_str(), std::ios_base::trunc);

		char *rest = _end_section;

		if (_section == 0)
		{
			char *insert_after = find_section("RISCOS !Boot", "Auto tasks");
			if (insert_after) insert_after = find_section_end(insert_after);
			if (insert_after) insert_after = next_line(insert_after);
			if (insert_after)
			{
				if (!*insert_after) file.write("\n",1);
				file.write(_file_contents, insert_after - _file_contents);
				rest = insert_after;
			} else
			{
				file.write(_file_contents, sizeof(_file_contents));
				rest = 0;
			}
			const char *start_text = "\n|Start RISCOS BootBoot 0.01 Boot\n";
			file.write(start_text, std::strlen(start_text));
		} else
		{
			char *next = next_line(_section);
			file.write(_file_contents, next - _file_contents);
		}
		for (std::vector<std::string>::iterator i = _apps.begin(); i != _apps.end(); ++i)
		{
			const std::string &app = *i;
			if (app[0] == '?')
			{
				// Didn't understand it, so write it verbatum
				file.write(app.c_str()+1, app.size()-1);
			} else
			{
				file.write("IfThere ", sizeof("IfThere ")-1);
				file.write(app.c_str(), app.size());
				file.write(" Then Filer_Boot ", sizeof(" Then Filer_Boot ")-1);
				file.write(app.c_str(), app.size());
			}
			file.write("\n",1);
		}
		if (!_end_section) file.write("|End\n", sizeof("|End\n")-1);
		if (rest) file.write(rest, std::strlen(rest));

		_modified = false;
	}
}

/**
 * Change the path name for read or write.
 *
 * This method is for unit testing and would not normally be used.
 * call rollback() to read the test file after this method.
 */
void look_at::use_test_path_name(const std::string &path_name)
{
	_path_name = path_name;
}

/**
 * See if file has the lookat section defined
 */
bool look_at::has_lookat_section() const
{
	return (_section != 0);
}

/**
 * Check if application is contained in the look ups
 *
 * @param app application to check
 * @returns true if application is contained in the look ups
 */
bool look_at::contains(const std::string &app) const
{
	std::string app_name = name_in_section(app);
	for(std::vector<std::string>::const_iterator i = _apps.begin(); i != _apps.end(); ++i)
	{
		if (app_name == *i) return true;
	}
	return false;
}

/**
 * Add application to up section
 *
 * @param app name of application to add
 * @returns true if app was added, false if it's already there.
 */
bool look_at::add(const std::string &app)
{
	std::string app_name = name_in_section(app);
	for(std::vector<std::string>::iterator i = _apps.begin(); i != _apps.end(); ++i)
	{
		if (app_name == *i) return false;
	}

	_apps.push_back(app_name);
	_modified = true;

	return true;
}

/**
 * Remove application from section
 *
 * @param app name of application to remove
 * @returns true if application was found and removed
 */
bool look_at::remove(const std::string &app)
{
	std::string app_name = name_in_section(app);
	for(std::vector<std::string>::iterator i = _apps.begin(); i != _apps.end(); ++i)
	{
		if (app_name == *i)
		{
			_apps.erase(i);
			_modified = true;
			return true;
		}
	}

	return false;
}

/**
 * Replace an application
 *
 * @param was_app application to replace
 * @param app application to replace it with
 * @returns true if the replace was done
 */
bool look_at::replace(const std::string &was_app, const std::string &app)
{
	std::string replace_app = name_in_section(was_app);
	for(std::vector<std::string>::iterator i = _apps.begin(); i != _apps.end(); ++i)
	{
		if (replace_app == *i)
		{
			*i = name_in_section(app);
			_modified = true;
			return true;
		}
	}

	return false;
}


/**
 * Check if exact name is contained in the look ups.
 *
 * Used for Unit testing to check that entries of format "Boot:^..."
 *
 * @param app application to check (is not standardized)
 * @returns true if application is contained in the look ups
 */
bool look_at::contains_raw(const std::string &app) const
{
	for(std::vector<std::string>::const_iterator i = _apps.begin(); i != _apps.end(); ++i)
	{
		if (app == *i) return true;
	}
	return false;
}

/**
 * Parse section we are interested into C++ structures
 */
void look_at::parse_section()
{
	_section = find_section("RISCOS BootBoot", "Boot");
	_end_section = 0;
	_apps.clear();
	if (_section)
	{
		char *line_end = std::strchr(_section,'\n');
		char *line_start = (line_end) ? line_end + 1 : 0;
		bool parsing = (line_start != 0);
		std::string word, app;
		char *token;

		while (parsing)
		{
			line_end = std::strchr(line_start, '\n');
			if (line_end == 0) line_end = line_start + std::strlen(line_start);
			token = line_start;

			parse_word(token, word);
			if (word == "|Start")
			{
				parsing = false;
			} else if (word == "|End")
			{
				_end_section = line_start;
				parsing = false;
			} else
			{
				if (word == "IfThere"
					&& parse_word(token, app)
					&& parse_word(token, word)
					&& (word == "Then")
					&& parse_word(token, word)
					&& (word == "Filer_Boot")
					&& parse_word(token, word)
					&& (word == app)
					&& !parse_word(token, word) // Check for end of line
					)
				{					
					// Just need app name
					_apps.push_back(app);
				} else
				{
					// Save whole line if we don't understand it
					if (line_end > line_start) word.assign(line_start, line_end - line_start);
					word.insert(0, "?");
					_apps.push_back(word);
				}

				if (*line_end)
				{
					line_start = line_end+1;
				} else parsing = false;
			}
		}
	}
}

/**
 * Parse a single word from the given location.
 *
 * Skips spaces, then a word is defined until the next
 * space, char 0 or line feed.
 *
 * @param pos location to start parsing - updated to position of character after word
 * @param word parsed out (empty if at end of line)
 * @returns true if a word is found.
 */
bool look_at::parse_word(char *&pos, std::string &word) const
{
	word.clear();
	while (*pos == ' ') pos++;
	char *word_start = pos;
	while (*pos && *pos != ' ' && *pos != '\n') pos++;
	if (pos > word_start) word.assign(word_start, pos - word_start);

	return !word.empty();
}
	
/**
 * Find section in the file.
 *
 * Section format is 
 * |Start <name> <version> <suffix>
 * where <version> is of form n.nn
 *
 * @param name name of section
 * @param suffix suffix name of section
 * @returns pointer to start of section or 0 if not found
 */
char *look_at::find_section(const char *name, const char *suffix)
{
	std::string prefix("|Start ");
	prefix += name;
	prefix += ' ';
	char *start_pos = _file_contents;
	char *section = 0;
	while (section == 0 && ((start_pos = std::strstr(start_pos, prefix.c_str())) != 0))
	{
		if (start_pos == _file_contents || *(start_pos-1) == '\n')
		{
			char *p = start_pos + prefix.size();
			while (*p == ' ') p++;
			// Check for and skip version
			if (isdigit(*p))
			{
				p++;
				while (isdigit(*p) || *p == '.') p++;
				while (*p == ' ') p++;
				if (std::strncmp(p, suffix, strlen(suffix)) == 0)
				{
					p += strlen(suffix);
					while (*p == ' ') p++;
					if (*p == 0 || *p == '\n') section = start_pos;
				}
			}
		}
		start_pos+= prefix.size();		
	}

	return section;
}

/**
 * Find the end of the section
 *
 * @param section location in section to find
 * @returns section end or 0 if not found
 */
char *look_at::find_section_end(char *section) const
{
	char *end = std::strstr(section, "\n|End");
	if (end)
	{
		// Check it's not missing
		char *next_start = std::strstr(section, "\n|Start");
		if (next_start && next_start < end) end = 0;
		else end++;
	}
	return end;
}

/**
 * Return adress of start of next line
 *
 * @param line line to find next line for
 * @returns start of next line
 */
char *look_at::next_line(char *line) const
{
	while (*line && *line != '\n') line++;
	if (*line == '\n') line++;
	return line;
}

/**
 * Generate the name used in the file.
 *
 * This usually involves replacing the start with Boot:^ if it's
 * on the boot drive
 *
 * @param app name of application
 * @returns name that will be stored in the file
 */
std::string look_at::name_in_section(const std::string &app) const
{
	if (_boot_drive.empty())
	{
		const_cast<look_at *>(this)->_boot_drive = canonicalise("Boot:^");
	}
	if (app.compare(0, _boot_drive.size(), _boot_drive) == 0)
	{
		std::string app_name("Boot:^");
		app_name += app.substr(_boot_drive.size());
		return app_name;
	} else
	{
		return app;
	}
}

/**
 * Dump apps array to cout - debugging helper
 */
void look_at::dump_apps() const
{
	for(std::vector<std::string>::const_iterator i = _apps.begin(); i != _apps.end(); ++i)
	{
		std::cout << *i << std::endl;
	}
}
