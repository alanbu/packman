
#include <string>
#include <vector>

// "Add to Apps": in <Choices$Write>.Boot.PreDesktop, at the end of the Acorn entries in the "ResApps" section;
// "Look at": in <Choices$Write>.Boot.Desktop, in the Acorn part of a new section named "Boot", to come immediately after the "Auto tasks" section;
// "Run": in <Choices$Write>.Boot.Desktop, in the Acorn part of a new section named "Run", to come immediately after the "Boot" section, or after "Auto tasks" if "Boot" does not exist.


/**
 * Base class to manipulate the RISC OS boot options files in Choices
 */
class boot_options_file
{
private:
	std::string _path_name;
	const char *_section_prefix;
	const char *_section_version;
	const char *_section_suffix;
	const char *_command;
	const char *_command2;
	char *_file_contents;
	char *_section;
	char *_end_section;
	std::vector<std::string> _apps;
	std::string _boot_drive;
	bool _modified;

public:
	boot_options_file(const char *file_name, const char *section_prefix, const char *section_version, const char *section_suffix, const char *command, const char *command2 = 0);
	virtual ~boot_options_file();

	void rollback();
	void commit();

	bool contains(const std::string &app) const;
	bool add(const std::string &app);
	bool remove(const std::string &app);
	bool replace(const std::string &was_app, const std::string &app);

	/**
	 * The path name to the location of the PreDesk file that
	 * contains the look at declarations
	 */
	const std::string &path_name() const {return _path_name;}
	
	void use_test_path_name(const std::string &path_name);
	bool has_section() const;
	bool modified() const {return _modified;}
	bool contains_raw(const std::string &app) const;

	void dump_apps() const;
	
protected:
	virtual char *find_insert_section() = 0;
	char *find_section(const char *name, const char *suffix);
	void parse_section();
	bool parse_word(char *&pos, std::string &word) const;
	char *find_section_end(char *section) const;
	char *next_line(char *line) const;
	std::string name_in_section(const std::string &app) const;
};

class look_at_options : public boot_options_file
{
public:
	look_at_options() : boot_options_file("Desktop", "RISCOS BootBoot", "0.01", "Boot","Filer_Boot") {}
	virtual ~look_at_options() {}
protected:
	virtual char *find_insert_section() {return find_section("RISCOS !Boot", "Auto tasks");};

};

class run_options : public boot_options_file
{
public:
	run_options() : boot_options_file("Desktop", "RISCOS BootRun", "0.01", "Run", "Filer_Boot", "Filer_Run") {}
	virtual ~run_options() {}

protected:
	virtual char *find_insert_section()
	{
		char *found = find_section("RISCOS BootBoot", "Boot");
		if (!found) found = find_section("RISCOS !Boot", "Auto tasks");
		return found;
	}

};

class add_to_apps_options : public boot_options_file
{
public:
	add_to_apps_options() : boot_options_file("PreDesktop","RISCOS BootApps","0.01","ResApps","AddApp") {}

protected:
	virtual char *find_insert_section() {return find_section("RISCOS !Boot", "ResApps");}	
};
