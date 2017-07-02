#ifndef FAS_LOG_H
#define FAS_LOG_H
#include <glog/logging.h>

namespace fas {
#define LOGGER_TRACE(MSG) (LOG(INFO) << MSG)
#define LOGGER_DEBUG(MSG) (LOG(INFO) << MSG)
#define LOGGER_INFO(MSG) (LOG(INFO) << MSG)
#define LOGGER_WARN(MSG) (LOG(WARNING) << MSG)
#define LOGGER_ERROR(MSG) (LOG(ERROR) << MSG)
#define LOGGER_FATAL(MSG) (LOG(FATAL) << MSG)
#define LOGGER_SYSERR(MSG) (LOG(ERROR) << MSG)
static const std::string CRLF = "\r\n";
bool InitGoogleLog(const std::string& log_dir);
void CloseGoogleLog();
class CommonLog {
public:
    int init(const std::string& log_dir, std::vector<std::string>& log_name) {
        InitGoogleLog(log_dir);
        return 0;
    }

    static CommonLog *instance();

    void closeLog() {
        google::ShutdownGoogleLogging();
    }
    ~CommonLog() {
        google::ShutdownGoogleLogging();
    }
private:
    static CommonLog *logger_;
};

}
#endif // FAS_LOG_H

