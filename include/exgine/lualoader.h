#pragma once

#define SOL_ALL_SAFETIES_ON 1
#define GLFW_INCLUDE_NONE

#include "exgine/datastore.h"
#include "exgine/engine.h"

#include <sol/sol.hpp>

namespace Exgine {
struct LoadedScript {
  bool enabled = true;
  int priority = 0;

  sol::safe_function preDraw = sol::nil;
  sol::safe_function ready = sol::nil;
  sol::safe_function update = sol::nil;
  sol::safe_function draw = sol::nil;
  sol::safe_function postDraw = sol::nil;
};

class Engine;

class LuaLoader {
public:
  LuaLoader(Engine *engine);

  // Execute a script once, no management but bindings are still available.
  void OneShot(const std::string &script);

  // Officially load a script into the engine, bindings are fully available.
  void LoadScript(const std::string &script);

  // Loads a directory of scripts into the engine, bindings are fully available.
  void LoadDirectory(const std::string &directory, bool saveToList = true);

  // Reloads all loaded directories. (NOTE: Unloads all scripts first,
  // individual files won't be reloaded)
  void ReloadDirectories();

  // Runs the ready function on every enabled lua script.
  void Ready();

  // Runs the update function on every enabled lua script.
  void Update();

  // Runs the preDraw function on every enabled lua script.
  void PreDraw();

  // Runs the draw function on every enabled lua script.
  void Draw();

  // Runs the postDraw function on every enabled lua script.
  void PostDraw();

  // Returns a reference to the lua state. (sol state, but raw can be obtained
  // through this)
  sol::state &GetLua();

  // Returns a reference to the loaded scripts.
  std::vector<LoadedScript> &GetScripts();

  // Clean up the lua state and all loaded scripts.
  void Cleanup();

private:
  sol::state lua;

  std::vector<LoadedScript> scripts;
  std::vector<std::string> loadedDirectories;

  DataStore datastore;

  Exgine::Engine *engine;
};
} // namespace Exgine
