#ifndef FAS_LOG_H
#define FAS_LOG_H
#include <ostream>
#include <sys/types.h>
#include <string>
#include <sstream>

#include <Types.h>


#include <boost/function.hpp>

bool defaultLogOutput(const char* data, int len);

/*!
 * \brief The Log class
 * Output the log of our pragram.
 * The Log class consist of six levels fllowing:
 * LogLevel::TRACE,
 * LogLevel::DEBUG
 * LogLevel::INFO
 * LogLevel::WARN
 * LogLevel::ERROR
 * LogLevel::FATAL
 */
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
  void logString(const std::string& str);

  void setOutput(default_output_t output);
};


template<typename T>
Log& operator<<(Log& log, T val) {
  log.getBuffer() << val;
  return log;
}

void Logger(std::string file, int line, std::string msg);
void Logger(std::string file, int line, Log::LogLevel level, std::string msg);
void Logger(std::string file, int line, Log::LogLevel level, std::string func, std::string msg);
void Logger(std::string file, int line, bool toAbort, std::string msg);

#define LOG_TRACE(msg) if (Log::logLevel() <= Log::TRACE) \
    Logger(__FILE__, __LINE__, Log::TRACE, __func__, msg)
#define LOG_DEBUG(msg) if (Log::logLevel() <= Log::DEBUG) \
    Logger(__FILE__, __LINE__, Log::DEBUG, __func__, msg)
#define LOG_INFO(msg) if (Log::logLevel() <= Log::INFO) \
    Logger(__FILE__, __LINE__, msg)
#define LOG_WARN(msg) Logger(__FILE__, __LINE__, Log::WARN, msg)
#define LOG_ERROR(msg) Logger(__FILE__, __LINE__, Log::ERROR, msg)
#define LOG_FATAL(msg) Logger(__FILE__, __LINE__, Log::FATAL, msg)
#define LOG_SYSERR(msg) Logger(__FILE__, __LINE__, false, msg)
#define LOG_SYSFATAL(msg) Logger(__FILE__, __LINE__, true, msg)


#endif // FAS_LOG_H

