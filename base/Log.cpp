#include <new>

#include <Log.h>
#include <Environment.h>

fas::CommonLog* fas::CommonLog::logger_ = nullptr;

bool fas::LoggerInit() {
  std::vector<std::string> logNames({"TRACE", "DEBUG", \
                                     "INFO", "ERROR", \
                                     "WARN", "SYS_ERROR", "FETAL"});
  return CommonLog::instance()->init("./conf/log.conf", logNames) == 0 ? true : false;
}

fas::CommonLog* fas::CommonLog::instance() {
    if (NULL == logger_) {
        logger_ = new(std::nothrow) CommonLog();
    }
    return logger_;
}


