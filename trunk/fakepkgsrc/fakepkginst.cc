/**
 * fakepkginstall.cc
 *
 * Fake the install of PackMan into the !Packages directory
 * shipped with it so users don't have to install it twice.
 */

const char *packages_dir = "Apps/Admin/!PackMan/Resources/!Packages";
const char *riscpkg_control = "RiscPkg/Control";
const char *riscpkg_copyright = "RiscPkg/Control";
const char *riscpkg_admin = "Apps/Admin";

#include <string>
#include <iostream>
#include <fstream>
#include <set>
#include <vector>
#include "sys/stat.h"
#include "dirent.h"

static void add_to_manifest(std::set<std::string> &mf, const std::string &dirname, const std::string &prefix);

int main()
{
	std::ifstream ifctrl(riscpkg_control);
	if (!ifctrl)
	{
		std::cerr << "Unable to open PackMan control file '" << riscpkg_control << "'" << std::endl;
		return -1;
	}

	// Create info directory
	std::string dirname(packages_dir);
	dirname += "/Info";
	mkdir(dirname.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
	dirname += "/PackMan";
	mkdir(dirname.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);

	std::string pathname(dirname);
	pathname += "/Control";

	std::ofstream ofctrl(pathname.c_str());
	if (!ofctrl)
	{
		std::cerr << "Unable to create PackMan Info control file at '" << pathname << "'" << std::endl;
		return -2;
	}

	// Copy control, but make a note of the version
	std::string line;
	std::string version;
	std::string control;
	ifctrl.peek();
	while (ifctrl && !ifctrl.eof())
	{
		std::getline(ifctrl, line);
		if (version.empty() && line.compare(0, 9, "Version: ") == 0)
		{
			version = line.substr(9);
			if (version[version.size()-1] < 32) version = version.erase(version.size()-1);
		}
		ofctrl << line << std::endl;
		control += line + "\n";
		ifctrl.peek();
	}

	ofctrl.close();
	ifctrl.close();

	if (version.empty())
	{
		std::cerr << "Unable to find version in control file '" << riscpkg_control << "'" << std::endl;
		return -4;
	}

	// Copy copyright
	pathname = dirname + "/Copyright";
	std::ifstream ifcpy(riscpkg_copyright);
	if (!ifcpy)
	{
		std::cerr << "Unable to open PackMan copyright file '" << riscpkg_copyright << "'" << std::endl;
		return -5;
	}
	std::ofstream ofcpy(pathname.c_str());
	if (!ofcpy)
	{
		std::cerr << "Unable to create PackMan Info copyright file at '" << pathname << "'" << std::endl;
		return -6;
	}

	ifcpy.peek();
	while (ifcpy && !ifcpy.eof())
	{
		std::getline(ifcpy, line);
		ofcpy << line << std::endl;
		ifcpy.peek();
	}

	ifcpy.close();
	ofcpy.close();

	// Build manifest
	std::set<std::string> mf;

	mf.insert("RiscPkg.Copyright");
	// Files that are generated by this program
	mf.insert("Apps.Admin.!PackMan.Resources.!Packages.Status");
	mf.insert("Apps.Admin.!PackMan.Resources.!Packages.Files");
	mf.insert("Apps.Admin.!PackMan.Resources.!Packages.Available");

	// Recursively add the files from PackMan
	add_to_manifest(mf, riscpkg_admin, "Apps.Admin.");

	pathname = dirname + "/Files";
	std::ofstream offiles(pathname.c_str());
	if (!offiles)
	{
		std::cerr << "Unable to create PackMan Info manifest file at '" << pathname << "'" << std::endl;
		return -7;
	}
	for (std::set<std::string>::iterator f = mf.begin(); f != mf.end(); ++f)
	{
		offiles << *f << std::endl;
	}
	offiles.close();

	// Save the control record as the Available list
	pathname = packages_dir;
	pathname += "/Available";

	std::ofstream ofavailable(pathname.c_str());
	if (!ofavailable)
	{
		std::cerr << "Unable to create PackMan Available file at '" << pathname << "'" << std::endl;
		return -3;
	}
	ofavailable << control;
	ofavailable.close();

	// Finally set up status with Packman installed
	pathname = packages_dir;
	pathname += "/Status";
	std::ofstream ofstat(pathname.c_str());
	if (!ofstat)
	{
		std::cerr << "Unable to create Packages Status file at '" << pathname << "'" << std::endl;
		return -8;
	}
	ofstat << "PackMan\t" << version << "\tinstalled\t";
	ofstat.close();

	std::cout << "Resources.Packages updated for installed PackMan" << std::endl;

	return 0;
}

/**
 * Recursively added items to the manifest
 */
void add_to_manifest(std::set<std::string> &mf, const std::string &dirname, const std::string &prefix)
{
	std::vector<std::string> subdirs;

	DIR *dir = opendir(dirname.c_str());
	if (dir == NULL)
	{
		std::cerr << "Failed to open directory " << dirname << std::endl;
	} else
	{
		struct dirent *entry;
		while((entry = readdir(dir)) != 0)
		{
			if (entry->d_name[0] != '.')
			{
				if (entry->d_type == DT_DIR) subdirs.push_back(entry->d_name);
				else
				{
#ifndef __riscos__
					std::string fname(entry->d_name);
					std::string::size_type dot_pos;
					while ((dot_pos = fname.find('.')) != std::string::npos) fname[dot_pos] = '/';
					std::string::size_type comma_pos = fname.rfind(',');
					if (comma_pos != std::string::npos) fname = fname.substr(0, comma_pos);
					mf.insert(prefix + fname);
#else
					mf.insert(prefix + entry->d_name);
#endif
				}
			}
		}
		closedir(dir);
	}
	std::string subdirname, subprefix;
	for(std::vector<std::string>::iterator d = subdirs.begin(); d!= subdirs.end(); ++d)
	{
		subdirname = dirname + "/" + *d;
		subprefix = prefix + *d + ".";
		add_to_manifest(mf, subdirname, subprefix);
	}
}
