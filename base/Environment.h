#ifndef FAS_ENVIRONMENT_H
#define FAS_ENVIRONMENT_H
#include <map>
#include <string>

#include <Reloader.h>
#include <Log.h>

namespace fas {

class Environment {
public:
    static Environment* instance();
    ~Environment();
    static bool init();
    bool check_load();
    bool insertReloader(const std::string& info, Reloader *loader);
    Reloader* getReloader(const std::string& info) const;
    Reloader* getReloader(const std::string& info);
private:
    class destroy_env {
        public:
            ~destroy_env() {
                if (Environment::env_) {
                    delete Environment::env_;
                }
            }
    };

    bool load_files();
    Environment() = default;
    static Environment *env_;
    Environment(const Environment&);
    Environment& operator=(const Environment&);

    std::map<std::string, Reloader*> reloaders_;
    static destroy_env ds_env;
};

}

fas::Environment* GET_ENV();
bool ENV_INIT();
void ENV_DESTROY();
#endif // FAS_ENVIRONMENT_H
