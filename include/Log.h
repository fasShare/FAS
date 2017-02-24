#ifndef FAS_LOG_H
#define FAS_LOG_H
#include <boost/function.hpp>
#include <ostream>
#include <string>
#include <sys/types.h>
#include <sstream>
#include <Default.h>

using std::string;
using std::ostream;
using std::ostringstream;

bool defaultLogOutput(const char* data, int len);

class Log{
private:
  typedef boost::function<bool (const char* data, int len)> default_output_t;
  default_output_t output_;
  std::ostringstream buffer_;
public:
  const static char *log_endl;
  enum LogLevel {
   TRACE,
   DEBUG,
   INFO,
   WARN,
   ERROR,
   FATAL,
   NUM_LOG_LEVELS,
   };

  Log() {
    output_ = defaultLogOutput;
  }

  std::ostringstream& getBuffer();

  bool fflush();

  static LogLevel logLevel();

  static void Logger(std::string file, int line);
  static void Logger(std::string file, int line, LogLevel level);
  static void Logger(std::string file, int line, LogLevel level, std::string func);
  static void Logger(std::string file, int line, bool toAbort);

  void logInt(int val);
  void loguInt(uint val);
  void logChar(char val);
  void loguChar(uchar val);
  void logChars(const char* chars, int len);
  void logFloat(float val);
  void logDouble(double val);
  void logString(const string& str);

  void setOutput(default_output_t output);
};


template<typename T>
Log& operator<<(Log& log, T val) {
  log.getBuffer() << val;
  return log;
}

template<typename T>
void Logger(std::string file, int line, T msg) {
  Log log;
  log << file << " " << line << " " << msg << Log::log_endl;
  log.fflush();
}
template<typename T>
void Logger(std::string file, int line, Log::LogLevel level, T msg) {
  Log log;
  log << file << " " << line << " " <<  level << " " << msg << Log::log_endl;
  log.fflush();
}
template<typename T>
void Logger(std::string file, int line, Log::LogLevel level, std::string func, T msg) {
  Log log;
  log << file << " " << line << " " <<  level << " " << func << " " << msg << Log::log_endl;
  log.fflush();
}
template<typename T>
void Logger(std::string file, int line, bool toAbort, T msg) {
  Log log;
  log << file << " " << line << " " << msg << Log::log_endl;
  log.fflush();

  if (toAbort) {
    ::abort();
  }
}


#define LOG_TRACE(msg) if (Log::logLevel() <= Log::TRACE) \
    Logger(__FILE__, __LINE__, Log::TRACE, __func__, #msg)
#define LOG_DEBUG(msg) if (Log::logLevel() <= Log::DEBUG) \
    Logger(__FILE__, __LINE__, Log::DEBUG, __func__, #msg)
#define LOG_INFO(msg) if (Log::logLevel() <= Log::INFO) \
    Logger(__FILE__, __LINE__, #msg)
#define LOG_WARN(msg) Logger(__FILE__, __LINE__, Log::WARN, #msg)
#define LOG_ERROR(msg) Logger(__FILE__, __LINE__, Log::ERROR, #msg)
#define LOG_FATAL(msg) Logger(__FILE__, __LINE__, Log::FATAL, #msg)
#define LOG_SYSERR(msg) Logger(__FILE__, __LINE__, false, #msg)
#define LOG_SYSFATAL(msg) Logger(__FILE__, __LINE__, true, #msg)


#endif // FAS_LOG_H

