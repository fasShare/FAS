#ifndef FAS_UTILFILE_H
#define FAS_UTILFILE_H
#include <string>
#include <sys/stat.h>

namespace fas {

namespace utils {

bool GetFileStat(const std::string& filename, struct stat *buf);
bool IsRegularFile(struct stat *buf);
bool IsDirectory(struct stat *buf);
off_t FileSizeInBytes(struct stat *buf);

}

}

#endif // UTILFILE_H

