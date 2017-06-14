#ifndef FAS_ENVIRONMENT_H
#define FAS_ENVIRONMENT_H
#include <reloader.h>
#include <Log.h>
#include <FasInfo.h>

#define GET_ENV() (Environment::instance())
#define ENV_INIT() (GET_ENV()->init())
#define GET_FAS_INFO() (GET_ENV()->getFasInfoLoader()->get_content())

typedef reloader_t<FasInfo> FasInfoLoader;

class Environment
{
public:
  static Environment* instance();

  static bool init();

  bool check_load() {
    int ret = 0;
    #define RELOAD_FILE(TYPE_P,NAME,arg...) \
      do{ \
        if (NULL != TYPE_P && TYPE_P->need_reload()) { \
          if (TYPE_P->reload(arg) >= 0) { \
            LOGGER_WARN << NAME << " reload succ" << fas::Log::CLRF;\
          } else { \
            ret = -1; \
            LOGGER_ERROR << NAME << " reload failed,please check" << fas::Log::CLRF;\
          } \
        } \
      }while(0)

    RELOAD_FILE(fasInfoLoader_, "./conf/fas.conf");
    if (ret < 0) {
      return false;
    }
    return true;
  }

#define GET_RELOAD(TYPE, NAME, P_TYPE) \
  TYPE* NAME() { \
    return P_TYPE; \
   }
  GET_RELOAD(FasInfoLoader, getFasInfoLoader, fasInfoLoader_)

private:
  bool load_files() {

#define LOAD_FILE_CONTENT(TYPE_P,C_TYPE_P,FPATH,arg...) \
    do{ \
      TYPE_P = new(std::nothrow) C_TYPE_P(FPATH);\
      if (NULL == TYPE_P) { \
        LOGGER_ERROR << "new failed: " << fas::Log::CLRF;\
        return false; \
      } \
      if (TYPE_P->load(arg) < 0) { \
        LOGGER_ERROR << "load file failed: " << fas::Log::CLRF;\
        return false; \
       } \
       LOGGER_DEBUG << "load file suss, file:" << fas::Log::CLRF;\
     }while(0)

#define CREATE_RELOADER(TYPE_P,C_TYPE_P,FPATH) \
    do{ \
      TYPE_P = new(std::nothrow) C_TYPE_P(FPATH);\
      if (NULL == TYPE_P) { \
        LOGGER_ERROR << "new failed: " << fas::Log::CLRF;\
        return false; \
      } \
      LOGGER_DEBUG << "create reloader suss, file:" << FPATH << fas::Log::CLRF;\
    }while(0)

    LOAD_FILE_CONTENT(fasInfoLoader_, FasInfoLoader, "./conf/fas.conf");

    return true;
  }

  static Environment *env_;
  Environment() {
  }
  Environment(const Environment&);
  Environment& operator=(const Environment&);

  FasInfoLoader *fasInfoLoader_;
};



#endif // FAS_ENVIRONMENT_H
