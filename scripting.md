# lua scripting

- scripts should be put in the `scripts` directory that gets created on launch
- only scripts with a proper `.lua` extension will get executed

## functions

### register functions to be ran

- `fn` is a lua function
- `key` is an integer, globally defined (below)

```lua
-- registers a function to run once on script load when the game is started
register_once(fn)

-- registers a function to run every tick (100 times/second)
register_tick(fn)

-- registers a function to run as long as a certain key is held
register_key_held(key, fn)

-- registers a function to run once when a key has been pressed
register_key_pressed(key, fn)
```

### vectors

- `vec2` and `vec3` are defined, with arithmetic and some helper methods (`dot`, `length`, `normalized` and string representation)
- `vec2` can be constructed with `vec2()`, `vec2(0)`, and `vec2(0, 0)`
- `vec3` can be constructed with `vec3()`, `vec3(0)`, and `vec3(0, 0, 0)`

### input

```lua
-- moves the mouse by the given vec2 (positive x is right, positive y is down)
mouse_move(coords)

-- presses the left mouse button (does not release it!)
mouse_left_press()

-- releases the left mouse button, if it has been pressed
mouse_left_release()
```

### player-related

- player indices range from 1 to 64, inclusive

```lua
-- returns a vec3 of the in-world position of the given player
player_position(index)
-- returns a vec3 of the in-world position of the local player
local_player_position()
-- returns a vec2 of the screen position of the given player
player_screen_position(index)
-- returns true if the given player is the local player
player_is_local(index)
```

## keys

- keys are defined in the global `key` object
- keys are named as `key.key_a`, `key.key_space`
- mouse buttons are named as `key.mouse_left`
- if unsure, look at include/key_code.hpp
