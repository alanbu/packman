// Utilities to help testing files

#include <string>
std::string canonicalise(const std::string& pathname);


bool compare_files(const char *file1, const char *file2);
void copy_file(const char *source, const char *target);
