#include "gtest-fixture/environ.hpp"
#include <cstdio>
#include <cstdlib>
#include <stdexcept>


using testing::fixture::EnvironFixture;
using std::string;
using std::snprintf;
using std::invalid_argument;


std::map<std::string, std::unique_ptr<char>> EnvironFixture::global;


EnvironFixture::~EnvironFixture() {
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


string EnvironFixture::GetEnv(const std::string &name, const std::string& fallback) {
    const auto* value{std::getenv(name.c_str())};
    return value ? value : fallback;
}


void EnvironFixture::SetEnv(const std::string& name, const std::string& value) {
    SaveEnv(name, value.c_str());
    putenv(local[name].get());
}


void EnvironFixture::DeleteEnv(const std::string& name) {
    const auto it{local.find(name)};
    if (it == local.end()) {
        SaveEnv(name);
    }
    else {
        it->second = nullptr;
    }
    unsetenv(name.c_str());
}


char* EnvironFixture::EnvStr(const std::string &name, const std::string &value) {
    const size_t strlen{name.length() + value.length() + 2};
    auto* env{new char[strlen]};
    snprintf(env, strlen, "%s=%s", name.c_str(), value.c_str());
    return env;  // caller assumes ownership
}


void testing::fixture::EnvironFixture::SaveEnv(const string& name, const char* value) {
    const auto* current{std::getenv(name.c_str())};
    if (current and global.find(name) == global.end()) {
        // Save the original variable so that it can be restored later.
        global[name].reset(EnvStr(name, current));
    }
    auto* env{value ? EnvStr(name, value) : nullptr};
    local[name].reset(env);  // object takes ownership
}
