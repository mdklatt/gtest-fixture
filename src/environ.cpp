#include "gtest-infra/environ.hpp"
#include <cstdio>
#include <cstdlib>
#include <stdexcept>


using namespace testing::infra::environ;
using std::string;
using std::snprintf;
using std::invalid_argument;


std::map<std::string, std::unique_ptr<char>> Environ::global;


Environ::~Environ() {
    for (auto& item: local) {
        // Roll back all local environment changes.
        auto it{global.find(item.first)};
        if (it != global.end()) {
            // Restore variable to its previous value. The underlying pointer
            // has static duration and remains valid for the life of the
            // application.
            putenv(it->second.get());
        }
        else {
            // Variable did not exist in original environment, so remove it.
            unsetenv(item.first.c_str());
        }
    }
}


string Environ::get(const std::string &name, const std::string& fallback) {
    const auto* value{std::getenv(name.c_str())};
    return value ? value : fallback;
}


void Environ::set(const std::string& name, const std::string& value) {
    save(name);
    const auto env{str(name, value)};
    local[name].reset(env);
    putenv(env);
}


void Environ::unset(const std::string& name) {
    save(name);
    local[name] = nullptr;
    unsetenv(name.c_str());
}


char* Environ::str(const std::string &name, const std::string& value) {
    const size_t strlen{name.length() + value.length() + 2};
    auto* env{new char[strlen]};
    snprintf(env, strlen, "%s=%s", name.c_str(), value.c_str());
    return env;  // caller assumes ownership
}


void Environ::save(const std::string &name) {
    if (local.find(name) != local.end()) {
        // Don't save variable that has already been modified.
        return;
    }
    const auto* current{std::getenv(name.c_str())};
    if (current) {
        // Save the original variable so that it can be restored later. This
        // must be static storage because it must remain valid for the life
        // of the application.
        global.emplace(name, str(name, current));
    }
}
