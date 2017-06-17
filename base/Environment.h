#ifndef FAS_ENVIRONMENT_H
#define FAS_ENVIRONMENT_H
#include <reloader.h>
#include <Log.h>
#include <FasInfo.h>

#define GET_ENV() (fas::Environment::instance())
#define ENV_INIT() (GET_ENV()->init())
#define GET_FAS_INFO() (GET_ENV()->getFasInfoLoader()->get_content())

namespace fas {

typedef reloader_t<FasInfo> FasInfoLoader;

class Environment
{
public:
  static Environment* instance();

  static bool init();

  bool check_load();


#define GET_RELOAD(TYPE, NAME, P_TYPE) \
  TYPE* NAME() { \
    return P_TYPE; \
   }

GET_RELOAD(FasInfoLoader, getFasInfoLoader, fasInfoLoader_)

private:
  bool load_files();

  static Environment *env_;
  Environment() {
  }
  Environment(const Environment&);
  Environment& operator=(const Environment&);

  FasInfoLoader *fasInfoLoader_;
};

}

#endif // FAS_ENVIRONMENT_H
