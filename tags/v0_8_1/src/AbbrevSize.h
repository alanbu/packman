/*
 * AbbrevSize.h
 *
 *  Created on: 30 Sep 2011
 *      Author: alanb
 */

#ifndef ABBREVSIZE_H_
#define ABBREVSIZE_H_

#include <sstream>

/**
 * Abbreviate a number of bytes
 */
inline std::string abbrev_size(long long size)
{
	std::string unit[] = {"B", "KB", "MB", "GB", "TB"};
	long long units = size;
	int unittype = 0;
	while ((units >= 4096) && (unittype < 4))
	{
	 units >>= 10;
	 ++unittype;
	}

	std::ostringstream oss;
	oss << units << " " << unit[unittype];

	return oss.str();
}


#endif /* ABBREVSIZE_H_ */
