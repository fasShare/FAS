#include <utility>

#include <Environment.h>
#include <FasInfoLoader.h>
#include <Log.h>

fas::Environment* fas::Environment::env_ = nullptr;

fas::Environment* fas::Environment::instance() {
    if (env_ == nullptr) {
        env_ = new Environment;
    }
    return env_;
}

bool fas::Environment::init(const std::string& log_dir) {
    if (!Environment::instance()) {
        return false;
    }
    if (fas::InitGoogleLog(log_dir) == false) {
        return false;
    }
    FasInfoLoader *loader = new (std::nothrow) FasInfoLoader("./conf/fas.conf");
    if (!env_->insertReloader("fasinfo", loader)) {
        LOGGER_ERROR("Environment insert FasInfoLoader error.");
        return false;
    }
    if (env_->load_files() == false) {
        LOGGER_ERROR("env_->load_files() == false");
        return false;
    }
    return true;
}

bool fas::Environment::insertReloader(const std::string& info, Reloader *loader) {
    if (info.empty()) {
        LOGGER_ERROR("inserted info empty.");
        return false;
    } else {
        if (loader == nullptr) {
            LOGGER_ERROR("inserted loader == nullptr");
            return false;
        }
    }
    auto retp = reloaders_.insert(std::make_pair(info, loader));
    if (!retp.second) {
        LOGGER_ERROR("inserted loader error. loader of " << loader->get_fname());
        return false;
    }
    return true;
}

bool fas::Environment::load_files() {  
    int ret = 0;

    for (auto iter = reloaders_.begin(); iter != reloaders_.end(); ++iter) {
        if (iter->second) {
            if (iter->second->load() >= 0) {
                LOGGER_TRACE("load " << iter->second->get_fname() << " success.");
            } else {
                LOGGER_TRACE("load " << iter->second->get_fname() << " failed.");
                ret -= 1;
            }
        } else {
            LOGGER_TRACE("Info " << iter->first << "'s reloader is null.");
            ret -= 1;
        }
    }

    if (ret < 0) {
        LOGGER_TRACE("load  ret = " << ret);
        return false;
    }
    return true;
}

bool fas::Environment::check_load() {
    int ret = 0;

    for (auto iter = reloaders_.begin(); iter != reloaders_.end(); ++iter) {
        if (iter->second && iter->second->need_reload()) {
            if (iter->second->reload() >= 0) {
                LOGGER_TRACE("Relaod " << iter->second->get_fname() << " success.");
            } else {
                LOGGER_TRACE("Relaod " << iter->second->get_fname() << " failed.");
                ret -= 1;
            }
        }
    }

    if (ret < 0) {
        LOGGER_TRACE("check_load  ret = " << ret);
        return false;
    }
    return true;
}

fas::Reloader* fas::Environment::getReloader(const std::string& info) const {
    auto iter = reloaders_.find(info);
    if (iter == reloaders_.end()) {
        return nullptr;
    }
    return iter->second;
}

fas::Reloader* fas::Environment::getReloader(const std::string& info) {
    auto iter = reloaders_.find(info);
    if (iter == reloaders_.end()) {
        return nullptr;
    }
    return iter->second;
}

fas::Environment* GET_ENV() {
    return fas::Environment::instance();
}
bool ENV_INIT(const std::string& log_dir) {
    return GET_ENV()->init(log_dir);
}
