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
            ::putenv(it->second.get());
        }
        else {
            // Variable did not exist in original environment, so remove it.
            it->second.reset(nullptr);
        }
    }
}


string EnvironFixture::getenv(const string& name, const std::string& fallback) const {
    const auto* value{std::getenv(name.c_str())};
    return value ? value : fallback;
}


void EnvironFixture::setenv(const string& name, const string& value) {
    const auto* current{std::getenv(name.c_str())};
    if (current and global.find(name) == global.end()) {
        // Save the original variable so that it can be restored later. This
        // storage has static duration that is shared by all fixtures, so it
        // will remain valid for the lifetime of the application.
        global[name].reset(newenv(name, current));
    }
    local[name].reset(newenv(name, value));  // fixture has ownership
    ::putenv(local[name].get());
}


void EnvironFixture::delenv(const string& name) {
    const auto it{local.find(name)};
    if (it == local.end()) {
        // Need to take ownership and then delete it.
        setenv(name);
        delenv(name);
    }
    else {
        // Fixture already has ownership, nullify the environment pointer.
        it->second.reset(nullptr);
    }
}


char* EnvironFixture::newenv(const string& name, const string& value) {
    const size_t strlen{name.length() + value.length() + 2};
    auto* env{new char[strlen]};
    snprintf(env, strlen, "%s=%s", name.c_str(), value.c_str());
    return env;  // caller assumes ownership
}
