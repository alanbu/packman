
#include <string>
#include <vector>

/**
 * Class to manipulate the RISC OS look at on boot items
 */
 
class look_at
{
private:
	std::string _path_name;
	char *_file_contents;
	char *_section;
	char *_end_section;
	std::vector<std::string> _apps;
	std::string _boot_drive;
	bool _modified;

public:
	look_at();
	~look_at();

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
	bool has_lookat_section() const;
	bool modified() const {return _modified;}
	bool contains_raw(const std::string &app) const;

	void dump_apps() const;
	
private:
	char *find_section(const char *name, const char *suffix);
	void parse_section();
	bool parse_word(char *&pos, std::string &word) const;
	char *find_section_end(char *section) const;
	char *next_line(char *line) const;
	std::string name_in_section(const std::string &app) const;
};

