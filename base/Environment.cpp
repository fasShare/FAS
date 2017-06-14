#include "Environment.h"

Environment* Environment::env_ = nullptr;

Environment* Environment::instance() {
  if (env_ == nullptr) {
    env_ = new Environment;
  }
  return env_;
}

bool Environment::init() {
  if (!Environment::instance()) {
    return false;
  }
  return env_->load_files();
}
