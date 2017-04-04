#include <Log.h>


int main() {

  fas::Log().LOG() << "FAS " << fas::Log::LogLevel::DEBUG  << fas::Log::CLRF;

  LOGGER_DEBUG << "Debug" << 0 << fas::Log::CLRF;

  LOGGER_TRACE << "Trace" << 1 << fas::Log::CLRF;

  LOGGER_ERROR << "Error" << 2 << fas::Log::CLRF;

  LOGGER_INFO << "Info"   << 3 << fas::Log::CLRF;

  LOGGER_WARN << "Warn"  << 4 << fas::Log::CLRF;

  LOGGER_FATAL << "Fatal" << 5 << fas::Log::CLRF;

  LOGGER_SYSERR << "SysErr" << 6 << fas::Log::CLRF;
  // 测试程序会abort
  LOGGER_SYSFATAL << "SysFatal" << " program will be done." << fas::Log::CLRF;
}
