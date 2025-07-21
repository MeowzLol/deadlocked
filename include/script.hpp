#pragma once

#include <glm/glm.hpp>
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <string>
#include <utility>
#include <vector>

#include "key_code.hpp"

struct Script {
    sol::state lua;
    std::vector<std::string> scripts;

    std::vector<sol::function> exec_once;
    std::vector<sol::function> exec_tick;
    std::vector<std::pair<KeyCode, sol::function>> exec_key_held;
    std::vector<std::pair<KeyCode, sol::function>> exec_key_pressed;

    Script();

  private:
    void RegisterVecs();
    void RegisterFunctions();
    void RegisterConstants();
    void RegisterPlayer();
};

extern Script script;
