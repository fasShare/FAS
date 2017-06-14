#ifndef FAS_LOG_H
#define FAS_LOG_H
#include <ostream>
#include <sys/types.h>
#include <string>
#include <sstream>
#include <pthread.h>


#include <Buffer.h>
#include <Thread.h>


#include <boost/function.hpp>
#include <boost/implicit_cast.hpp>

namespace fas {

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
   TRACE = 1,
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

  static LogLevel getLogLevel();

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

template<typename T>
Log& operator<<(Log& log, T val) {
  log.getBuffer() << val;
  return log.fflush();
}

#define LOGGER_TRACE  fas::Log(__FILE__, __LINE__, fas::Log::TRACE, __func__).LOG()
#define LOGGER_DEBUG  fas::Log(__FILE__, __LINE__, fas::Log::DEBUG, __func__).LOG()
#define LOGGER_INFO   fas::Log(__FILE__, __LINE__).LOG()
#define LOGGER_WARN fas::Log(__FILE__, __LINE__, fas::Log::WARN).LOG()
#define LOGGER_ERROR fas::Log(__FILE__, __LINE__, fas::Log::ERROR).LOG()
#define LOGGER_FATAL fas::Log(__FILE__, __LINE__, fas::Log::FATAL).LOG()
#define LOGGER_SYSERR fas::Log(__FILE__, __LINE__, false).LOG()
#define LOGGER_SYSFATAL fas::Log(__FILE__, __LINE__, true).LOG()

}
#endif // FAS_LOG_H

