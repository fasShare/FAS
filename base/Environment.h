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
    static bool init(const std::string& log_dir);
    bool check_load();
    bool insertReloader(const std::string& info, Reloader *loader);
    Reloader* getReloader(const std::string& info) const;
    Reloader* getReloader(const std::string& info);
private:
    bool load_files();
    Environment() = default;
    static Environment *env_;
    Environment(const Environment&);
    Environment& operator=(const Environment&);

    std::map<std::string, Reloader*> reloaders_;
};

}

fas::Environment* GET_ENV();
bool ENV_INIT(const std::string& log_dir);

#endif // FAS_ENVIRONMENT_H
