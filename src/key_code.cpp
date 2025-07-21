#include "key_code.hpp"

bool IsKey(i32 key) {
    return key_code_names.find(static_cast<KeyCode>(key)) != key_code_names.end();
}
