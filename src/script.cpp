#include "script.hpp"

#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <vector>

#include "cs2/player.hpp"
#include "glm/fwd.hpp"
#include "key_code.hpp"
#include "mouse.hpp"
#include "types.hpp"

Script script;

Script::Script() {
    lua.open_libraries(sol::lib::base, sol::lib::string, sol::lib::math);

    RegisterVecs();
    RegisterFunctions();
    RegisterConstants();
    RegisterPlayer();

    const auto exe = std::filesystem::canonical("/proc/self/exe");
    const auto path = exe.parent_path() / "scripts";
    if (!std::filesystem::exists(path)) {
        std::filesystem::create_directory(path);
    }

    for (const auto &entry : std::filesystem::directory_iterator(path)) {
        if (!entry.is_regular_file()) {
            continue;
        }
        if (entry.path().filename().extension() == ".lua") {
            std::ostringstream input;
            std::ifstream file(entry.path());
            input << file.rdbuf();
            scripts.push_back(input.str());
        }
    }

    for (const auto &script : scripts) {
        lua.script(script);
    }
}

void Script::RegisterVecs() {
    auto v2 = lua.new_usertype<glm::vec2>(
        "vec2", sol::call_constructor,
        sol::constructors<glm::vec2(), glm::vec2(f32), glm::vec2(f32, f32)>());
    v2.set("x", &glm::vec2::x);
    v2.set("y", &glm::vec2::y);
    v2.set(
        sol::meta_function::addition, [](const glm::vec2 &a, const glm::vec2 &b) { return a + b; });
    v2.set(sol::meta_function::subtraction, [](const glm::vec2 &a, const glm::vec2 &b) {
        return a - b;
    });
    v2.set(sol::meta_function::unary_minus, [](const glm::vec2 &a) { return -a; });
    v2.set(
        sol::meta_function::multiplication, sol::overload(
                                                [](const glm::vec2 &a, float s) { return a * s; },
                                                [](float s, const glm::vec2 &a) { return a * s; }));
    v2.set(sol::meta_function::division, [](const glm::vec2 &a, float s) { return a / s; });
    v2.set(sol::meta_function::equal_to, [](const glm::vec2 &a, const glm::vec2 &b) {
        return a == b;
    });
    v2.set(sol::meta_function::to_string, [](const glm::vec2 &a) {
        std::ostringstream o;
        o << "vec2(" << a.x << ", " << a.y << ")";
        return o.str();
    });
    v2.set("dot", [](const glm::vec2 &a, const glm::vec2 &b) { return glm::dot(a, b); });
    v2.set("length", [](const glm::vec2 &a) { return glm::length(a); });
    v2.set("normalized", [](const glm::vec2 &a) { return glm::normalize(a); });

    auto v3 = lua.new_usertype<glm::vec3>(
        "vec3", sol::call_constructor,
        sol::constructors<glm::vec3(), glm::vec3(f32), glm::vec3(f32, f32, f32)>());
    v3.set("x", &glm::vec3::x);
    v3.set("y", &glm::vec3::y);
    v3.set("z", &glm::vec3::z);
    v3.set(
        sol::meta_function::addition, [](const glm::vec3 &a, const glm::vec3 &b) { return a + b; });
    v3.set(sol::meta_function::subtraction, [](const glm::vec3 &a, const glm::vec3 &b) {
        return a - b;
    });
    v3.set(sol::meta_function::unary_minus, [](const glm::vec3 &a) { return -a; });
    v3.set(
        sol::meta_function::multiplication, sol::overload(
                                                [](const glm::vec3 &a, float s) { return a * s; },
                                                [](float s, const glm::vec3 a) { return a * s; }));
    v3.set(sol::meta_function::division, [](const glm::vec3 &a, float s) { return a / s; });
    v3.set(sol::meta_function::equal_to, [](const glm::vec3 &a, const glm::vec3 &b) {
        return a == b;
    });
    v3.set(sol::meta_function::to_string, [](const glm::vec3 &a) {
        std::ostringstream o;
        o << "vec3(" << a.x << ", " << a.y << ", " << a.z << ")";
        return o.str();
    });
    v3.set("dot", [](const glm::vec3 &a, const glm::vec3 &b) { return glm::dot(a, b); });
    v3.set("cross", [](const glm::vec3 &a, const glm::vec3 &b) { return glm::cross(a, b); });
    v3.set("length", [](const glm::vec3 &a) { return glm::length(a); });
    v3.set("normalized", [](const glm::vec3 &a) { return glm::normalize(a); });
}

void Script::RegisterFunctions() {
    lua.set_function("register_once", [this](sol::function fn) { exec_once.push_back(fn); });
    lua.set_function("register_tick", [this](sol::function fn) { exec_tick.push_back(fn); });
    lua.set_function("register_key_held", [this](i32 key, sol::function fn) {
        if (!IsKey(key)) {
            throw std::runtime_error("key is not valid");
        }
        exec_key_held.emplace_back(static_cast<KeyCode>(key), fn);
    });
    lua.set_function("register_key_pressed", [this](i32 key, sol::function fn) {
        if (!IsKey(key)) {
            throw std::runtime_error("key is not valid");
        }
        exec_key_pressed.emplace_back(static_cast<KeyCode>(key), fn);
    });

    lua.set_function("mouse_move", [](const glm::vec2 &coords) { MouseMove(coords); });
    lua.set_function("mouse_left_press", []() { MouseLeftPress(); });
    lua.set_function("mouse_left_release", []() { MouseLeftRelease(); });
}

void Script::RegisterConstants() {
    auto keys = lua.create_table();
    for (const auto &[key, id] : key_names) {
        keys[key] = id;
    }
    lua.set("key", keys);
}

void Script::RegisterPlayer() {
    auto player = lua.new_usertype<Player>("player");
    player.set("index", [](u64 index) -> std::optional<Player> { return Player::Index(index); });
    player.set("local", []() { return Player::LocalPlayer(); });
    player.set("health", [](const Player &player) { return player.Health(); });
    player.set("armor", [](const Player &player) { return player.Armor(); });
    player.set("name", [](const Player &player) { return player.Name(); });
    player.set("team", [](const Player &player) { return player.Team(); });
    player.set("held_weapon", [](const Player &player) { return player.WeaponName(); });
    player.set("all_weapons", [](const Player &player) { return player.AllWeapons(); });
    player.set("position", [](const Player &player) { return player.Position(); });
    player.set("shots_fired", [](const Player &player) { return player.ShotsFired(); });
    player.set("is_valid", [](const Player &player) { return player.IsValid(); });
}
