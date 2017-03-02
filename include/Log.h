#ifndef FAS_LOG_H
#define FAS_LOG_H
#include <ostream>
#include <sys/types.h>
#include <string>
#include <sstream>

#include <Types.h>
#include <Buffer.h>


#include <boost/function.hpp>
#include <boost/implicit_cast.hpp>

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
public:
  typedef boost::function<bool (const char* data, int len)> default_output_t;
  const static char *CLRF;
  enum LogLevel {
   TRACE,
   DEBUG,
   INFO,
   WARN,
   ERROR,
   FATAL,
   NUM_LOG_LEVELS,
   };

  Log();
  Log(std::string file, int line);
  Log(std::string file, int line, Log::LogLevel level);
  Log(std::string file, int line, Log::LogLevel level, std::string func);
  Log(std::string file, int line, bool toAbort);
  ~Log();

  Log& fflush();

  static LogLevel logLevel();

  void setOutput(default_output_t output);

  std::ostringstream& getBuffer();

  Log& LOG();
private:
  LogLevel runTimeLevel_;
  std::string file_;
  int line_;
  std::string func_;
  bool abort_;
  default_output_t output_;
  // FIXME : use little buffer.
  std::ostringstream buffer_;
};

void Logger(std::string file, int line, bool toAbort, std::string msg);
void Logger(std::string file, int line, Log::LogLevel level, std::string func, std::string msg);
void Logger(std::string file, int line, Log::LogLevel level, std::string msg);
void Logger(std::string file, int line, std::string msg);

template<typename T>
Log& operator<<(Log& log, T val) {
  log.getBuffer() << val;
  return log.fflush();
}

#define LOGGER_TRACE  Log(__FILE__, __LINE__, Log::TRACE, __func__).LOG()
#define LOGGER_DEBUG  Log(__FILE__, __LINE__, Log::DEBUG, __func__).LOG()
#define LOGGER_INFO   Log(__FILE__, __LINE__).LOG()
#define LOGGER_WARN Log(__FILE__, __LINE__, Log::WARN).LOG()
#define LOGGER_ERROR Log(__FILE__, __LINE__, Log::ERROR).LOG()
#define LOGGER_FATAL Log(__FILE__, __LINE__, Log::FATAL).LOG()
#define LOGGER_SYSERR Log(__FILE__, __LINE__, false).LOG()
#define LOGGER_SYSFATAL Log(__FILE__, __LINE__, true).LOG()


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

