/*********************************************************************
* Copyright 2010-2015 Alan Buckley
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
 * SourceFile.cpp
 *
 *  Created on: 20 Nov 2015
 *      Author: alanb
 */

#include "SourcesFile.h"
#include "Choices.h"

#include "tbx/path.h"

#include <fstream>
#include <stdexcept>


SourcesFile::SourcesFile()
{
}

SourcesFile::~SourcesFile()
{
}

/**
 * Load sources from sources file
 */
void SourcesFile::load()
{
	// Read sources from file so we also get the disabled sources
	std::string sources_path(choices_read_path("Sources"));
	std::ifstream in(sources_path.c_str());
    // If we can't open them fall back to the default
	if (!in) in.open("<PackMan$Dir>.Resources.Sources");
	while (in&&!in.eof())
	{
		// Read line from input stream.
		std::string line;
		std::getline(in,line);

		// Strip comments and trailing spaces.
		std::string::size_type n=line.find('#');
		if (n!=std::string::npos)
		{
			n++;
			while(n < line.size() && std::isspace(line[n])) line.erase(n,1);
			if (n == line.size() || line.compare(n,3,"pkg") != 0) n = 0;
			else n = line.find('#',n+1 ); // Strip second set of comments
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
			_sources.push_back(std::make_pair(srcpath, true));
		} else if (srctype=="#pkg")
		{
			_sources.push_back(std::make_pair(srcpath, false));
		}

		// Check for end of file.
		in.peek();
	}
}

/**
 * Resave the sources file
 *
 * @throws std::runtime_error if save fails
 */
void SourcesFile::save()
{
	if (!Choices::ensure_choices_dir())
	{
		std::string msg("Unable to create ");
		msg += ChoicesDir;
		throw std::runtime_error(msg);
	} else
	{
		tbx::Path sources_path(choices_write_path("Sources"));

		std::ofstream sfile(sources_path.name());
		if (sfile)
		{
			sfile << "# Sources written by PackMan" << std::endl << std::endl;
			int num_entries = _sources.size();
			for (int i = 0; i < num_entries; i++)
			{
				if (!_sources[i].second) sfile << "#";
				sfile << "pkg " << _sources[i].first << std::endl;
			}
			sfile.close();
		}

		if (!sfile)
		{
			std::string msg("Unable to write sources file ");
			msg += sources_path.name();
			throw std::runtime_error(msg);
		}
	}
}

/**
 * Add a new url to the sources file
 *
 * @param url new url
 * @param enabled true if source is enabled
 */
void SourcesFile::add(const std::string &url, bool enabled)
{
	_sources.push_back(std::make_pair(url, true));
}

/**
 * Find url in sources file
 *
 * @param url URL to find
 * @returns index of url or -1 if not found
 */
int SourcesFile::find(const std::string &url) const
{
	for (unsigned int i = 0; i < _sources.size(); i++)
	{
		if (_sources[i].first == url) return i;
	}
	return -1;
}

/**
 * Set source URL for a specific index
 *
 * @param index index to change
 * @param url new URL
 */
void SourcesFile::url(int index, const std::string &url)
{
	_sources[index].first = url;
}
