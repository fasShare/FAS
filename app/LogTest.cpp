#include <Log.h>


int main() {
  Log().LOG() << "FAS" << Log::CLRF;

  LOGGER_DEBUG << "Debug" << 0 << Log::CLRF;

  LOGGER_TRACE << "Trace" << 1 << Log::CLRF;

  LOGGER_ERROR << "Error" << 2 << Log::CLRF;

  LOGGER_INFO << "Info"   << 3 << Log::CLRF;

  LOGGER_WARN << "Warn"  << 4 << Log::CLRF;

  LOGGER_FATAL << "Fatal" << 5 << Log::CLRF;

  LOGGER_SYSERR << "SysErr" << 6 << Log::CLRF;
  // 测试程序会abort
  LOGGER_SYSFATAL << "SysFatal" << " program will be done." << Log::CLRF;
}
