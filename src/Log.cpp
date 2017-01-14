#include <Log.h>

const char *Log::log_endl = "\n\r";

std::ostringstream& Log::get_buffer() {
  return buffer_;
}

bool Log::fflush() {
  if (!output_) {
    assert(false);
  }
  return output_(buffer_.str().c_str(), buffer_.str().size());
}

Log::LogLevel Log::logLevel() {
  return Log::LogLevel::DEBUG;
}


void Log::set_output(default_output_t output) {
  output_ = output;
}

bool defaultLogOutput(const char* data, int len) {
  int ret = ::write(STDOUT_FILENO, data, len);
  if (ret == -1) {
    // FIXME :
    return false;
  }
  return true;
}


