#include <Environment.h>
#include <Log.h>

fas::Environment* fas::Environment::env_ = nullptr;

fas::Environment* fas::Environment::instance() {
  if (env_ == nullptr) {
    env_ = new Environment;
  }
  return env_;
}

bool fas::Environment::init() {
  if (!Environment::instance()) {
    return false;
  }
  if (fas::LoggerInit() == false) {
    LOGGER_ERROR("fas::LoggerInit() == false");
    return false;
  }
  if (env_->load_files() == false) {
    LOGGER_ERROR("env_->load_files() == false");
    return false;
  }
  return true;
}

bool fas::Environment::load_files() {

#define LOAD_FILE_CONTENT(TYPE_P,C_TYPE_P,FPATH,arg...) \
  do{ \
    TYPE_P = new(std::nothrow) C_TYPE_P(FPATH);\
    if (NULL == TYPE_P) { \
      LOGGER_ERROR("new TYPE_P failed!");\
      return false; \
    } \
    if (TYPE_P->load(arg) < 0) { \
      LOGGER_ERROR("load file failed!");\
      return false; \
     } \
     LOGGER_DEBUG("load file suss, file:" << FPATH);\
   }while(0)

#define CREATE_RELOADER(TYPE_P,C_TYPE_P,FPATH) \
  do{ \
    TYPE_P = new(std::nothrow) C_TYPE_P(FPATH);\
    if (NULL == TYPE_P) { \
      LOGGER_ERROR("new TYPE_P failed!");\
      return false; \
    } \
    LOGGER_DEBUG("create reloader suss, file:" << FPATH);\
  }while(0)

  LOAD_FILE_CONTENT(fasInfoLoader_, FasInfoLoader, "./conf/fas.conf");

  return true;
}

bool fas::Environment::check_load() {
  int ret = 0;
  #define RELOAD_FILE(TYPE_P,NAME,arg...) \
    do{ \
      if (NULL != TYPE_P && TYPE_P->need_reload()) { \
        if (TYPE_P->reload(arg) >= 0) { \
          LOGGER_WARN(NAME << " reload succ");\
        } else { \
          ret = -1; \
          LOGGER_ERROR(NAME << " reload failed,please check");\
        } \
      } \
    }while(0)

  RELOAD_FILE(fasInfoLoader_, "./conf/fas.conf");
  if (ret < 0) {
    return false;
  }
  return true;
}
