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

int fas::CommonLog::init(const std::string& name, std::vector<std::string>& log_name) {
	if (name.empty()) {
		return -1;
	}

	log4cplus::initialize();
	log4cplus::helpers::LogLog::getLogLog()->setInternalDebugging(true);
	log4cplus::PropertyConfigurator::doConfigure(name);

#define CREATE_LOG_OBJECT(KEY, POINT) \
	if (log_name.end() != std::find(log_name.begin(), log_name.end(), #KEY)) { \
		log4cplus::Logger logger = log4cplus::Logger::getInstance(#KEY);\
		POINT = new log4cplus::Logger(logger); \
	}
	CREATE_LOG_OBJECT(TRACE, trace_)
	CREATE_LOG_OBJECT(DEBUG, debug_)
	CREATE_LOG_OBJECT(INFO, info_)
	CREATE_LOG_OBJECT(WARN, warn_)
	CREATE_LOG_OBJECT(ERROR, error_)
	CREATE_LOG_OBJECT(SYS_ERROR, sys_error_)
	CREATE_LOG_OBJECT(FETAL, fetal_)
	return 0;
}

