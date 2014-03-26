
#include "fileutils.h"
#include <swis.h>
#include <stdexcept>
#include <fstream>

inline void call_swi(int id, _kernel_swi_regs *regs)
{
	_kernel_oserror *err = _kernel_swi(id, regs, regs);
	if (err) throw std::runtime_error(err->errmess);
}

/**
 * canoncialise given filename
 *
 * @param pathname name of path to canonicalise
 * @returns canonicalised version
 */
std::string canonicalise(const std::string& pathname)
{
	// Calculate required buffer size.
	_kernel_swi_regs regs;
	regs.r[0]=37;
	regs.r[1]=(int)pathname.c_str();
	regs.r[2]=(int)0;
	regs.r[3]=(int)0;
	regs.r[4]=(int)0;
	regs.r[5]=0;

	call_swi(OS_FSControl,&regs);
	regs.r[5] = 1-regs.r[5];

	// Canonicalise pathname.
	char buffer[regs.r[5]];
	regs.r[2] = (int)buffer;
	call_swi(OS_FSControl, &regs);
	return std::string(buffer);
}

/**
 * Compare two files
 *
 * @param file1 path name of first file to compare
 * @param file2 path name of second file to compare
 * @returns true if contents are equal
 * @throws std::ios_base::failure if a file operation fails
 */
bool compare_files(const char *file1, const char *file2)
{
	std::ifstream f1;
	std::ifstream f2;

	f1.open(file1, std::ios_base::binary);
	f2.open(file2, std::ios_base::binary);

	if (!f1) throw std::runtime_error(std::string("Unable to open first file to compare: ") + file1);
	if (!f2) throw std::runtime_error(std::string("Unable to open second file to compare: ") + file2);

	f1.exceptions( std::ofstream::failbit | std::ofstream::badbit );
	f2.exceptions( std::ofstream::failbit | std::ofstream::badbit );

	f1.seekg(0, std::ios_base::end);
	int len = f1.tellg();
	f1.seekg(0, std::ios_base::beg);

	f2.seekg(0, std::ios_base::end);
	if ((int)f2.tellg() != len) return false; // Not the same size
	f2.seekg(0, std::ios_base::beg);

    char buffer1[len], buffer2[len];
	f1.read(buffer1, len);
	f2.read(buffer2, len);

	return (std::memcmp(buffer1, buffer2, len) == 0);
}

/**
 * Copy file overwriting destinations
 *
 * @param source file to copy
 * @param target destination
 * @throws std::ios_base::failure if a file operation fails
 */
void copy_file(const char *source, const char *target)
{
	std::ifstream sf;
	std::ofstream tf;

	sf.open(source, std::ios_base::binary);
	if (!sf) throw std::runtime_error(std::string("Unable to open source file to copy: ") + source);
	tf.open(target, std::ios_base::binary | std::ios_base::trunc);
	if (!tf) throw std::runtime_error(std::string("Unable to create target file in copy: ") + target);

	sf.exceptions( std::ofstream::failbit | std::ofstream::badbit );
	tf.exceptions( std::ofstream::failbit | std::ofstream::badbit );
	sf.seekg(0, std::ios_base::end);
	std::streamoff len = sf.tellg();
	sf.seekg(0, std::ios_base::beg);

    char buffer[len];
	sf.read(buffer, len);
	tf.write(buffer, len);
}
