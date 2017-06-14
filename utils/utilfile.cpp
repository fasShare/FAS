#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>


#include <utilfile.h>

bool fas::utils::GetFileStat(const std::string& filename, struct stat *st) {
  int ret = ::lstat(filename.c_str(), st);
  if (ret == -1) {
    return false;
  }
  return true;
}

bool fas::utils::IsRegularFile(struct stat *st) {
  if (S_ISREG(st->st_mode)) {
    return true;
  }
  return false;
}

bool fas::utils::IsDirectory(struct stat *st) {
  if (S_ISDIR(st->st_mode)) {
    return true;
  }
  return false;
}

off_t fas::utils::FileSizeInBytes(struct stat *st) {
  return st->st_size;
}
