#include <new>
#include <iostream>
#include <unistd.h>

#include <Log.h>

fas::CommonLog* fas::CommonLog::logger_ = nullptr;

fas::CommonLog* fas::CommonLog::instance() {
    if (NULL == logger_) {
        logger_ = new(std::nothrow) CommonLog();
    }
    return logger_;
}

bool fas::InitGoogleLog(const std::string& log_dir) {
    google::InitGoogleLogging("");
//    google::SetStderrLogging(google::GLOG_INFO); //设置级别高于 google::INFO 的日志同时输出到屏幕
//    google::SetStderrLogging(google::GLOG_FATAL);//设置级别高于 google::FATAL 的日志同时输出到屏幕
    FLAGS_colorlogtostderr = true; //设置输出到屏幕的日志显示相应颜色
    //FLAGS_servitysinglelog = true;// 用来按照等级区分log文件
    if (::access(log_dir.c_str(), F_OK|W_OK|R_OK)) {
        std::cerr << "fas has no permission of log dir "<< log_dir  << std::endl;
        return false;
    }

    google::SetLogDestination(google::GLOG_FATAL, (log_dir + "/log_fatal_").c_str()); // 设置 google::FATAL 级别的日志存储路径和文件名前缀
    google::SetLogDestination(google::GLOG_ERROR, (log_dir + "/log_error_").c_str()); //设置 google::ERROR 级别的日志存储路径和文件名前缀
    google::SetLogDestination(google::GLOG_WARNING, (log_dir + "/log_warning_").c_str()); //设置 google::WARNING 级别的日志存储路径和文件名前缀
    google::SetLogDestination(google::GLOG_INFO, (log_dir + "/log_info_").c_str()); //设置 google::INFO 级别的日志存储路径和文件名前缀
    FLAGS_logbufsecs = 0; //缓冲日志输出，默认为30秒，此处改为立即输出
    FLAGS_max_log_size = 100; //最大日志大小为 100MB
    FLAGS_stop_logging_if_full_disk = true; //当磁盘被写满时，停止日志输出
    google::SetLogFilenameExtension(".log"); //设置文件名扩展，如平台？或其它需要区分的信息
    //google::InstallFailureSignalHandler(); //捕捉 core dumped (linux)
    //google::InstallFailureWriter(&Log); //默认捕捉 SIGSEGV 信号信息输出会输出到 stderr (linux)
    return true;
}

//    google::ShutdownGoogleLogging();
