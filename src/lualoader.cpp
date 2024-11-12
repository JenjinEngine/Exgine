#define SOL_ALL_SAFETIES_ON 1
#define GLFW_INCLUDE_NONE

#include <exgine/lualoader.h>

#include <glm/glm.hpp>
#include <sol/raii.hpp>
#include <sol2_ImGui_Bindings/sol_ImGui.h>
#include <spdlog/fmt/bundled/color.h>
#include <spdlog/fmt/bundled/format.h>
#include <spdlog/spdlog.h>

#include <imgui.h>
#include <imgui_internal.h>

#include <filesystem>
#include <sstream>

using namespace Exgine;

std::string construct_string(sol::variadic_args va);

LuaLoader::LuaLoader(Engine *engine) : engine(engine) {
  spdlog::trace("LuaLoader::LuaLoader()");

  // base, table, string, math
  this->lua.open_libraries(sol::lib::base, sol::lib::table, sol::lib::string,
                           sol::lib::math);

  sol_ImGui::Init(this->lua);

  auto igt = this->lua["ImGui"];

  igt["ShowDemoWindow"] = &ImGui::ShowDemoWindow;

  igt["GetMainViewport"] = &ImGui::GetMainViewport;
  igt["SetNextWindowViewport"] = &ImGui::SetNextWindowViewport;
  igt["DockBuilderRemoveNode"] = &ImGui::DockBuilderRemoveNode;
  igt["DockBuilderAddNode"] = &ImGui::DockBuilderAddNode;
  igt["DockBuilderSplitNode"] = &ImGui::DockBuilderSplitNode;
  igt["DockBuilderDockWindow"] = &ImGui::DockBuilderDockWindow;
  igt["DockBuilderFinish"] = &ImGui::DockBuilderFinish;

  igt["GetIO"] = &ImGui::GetIO;

  // Disable GC (PERF: Fix this later)
  this->lua.change_gc_mode_generational(0, 0);

  // Register the trace, print, info, warn, error, and critical functions
  this->lua.set_function("trace", [](sol::variadic_args va) {
    spdlog::trace(
        "{}{}",
        fmt::format(fmt::emphasis::bold | fmt::fg(fmt::color::light_blue),
                    "[LUA] "),
        construct_string(va));
  });

  this->lua.set_function("print", [](sol::variadic_args va) {
    spdlog::info(
        "{}{}",
        fmt::format(fmt::emphasis::bold | fmt::fg(fmt::color::light_blue),
                    "[LUA] "),
        construct_string(va));
  });

  this->lua.set_function("info", [](sol::variadic_args va) {
    spdlog::info(
        "{}{}",
        fmt::format(fmt::emphasis::bold | fmt::fg(fmt::color::light_blue),
                    "[LUA] "),
        construct_string(va));
  });

  this->lua.set_function("warn", [](sol::variadic_args va) {
    spdlog::warn(
        "{}{}",
        fmt::format(fmt::emphasis::bold | fmt::fg(fmt::color::light_blue),
                    "[LUA] "),
        construct_string(va));
  });

  this->lua.set_function("error", [](sol::variadic_args va) {
    spdlog::error(
        "{}{}",
        fmt::format(fmt::emphasis::bold | fmt::fg(fmt::color::light_blue),
                    "[LUA] "),
        construct_string(va));
  });

  this->lua.set_function("critical", [](sol::variadic_args va) {
    spdlog::critical(
        "{}{}",
        fmt::format(fmt::emphasis::bold | fmt::fg(fmt::color::light_blue),
                    "[LUA] "),
        construct_string(va));
  });

  this->lua.safe_script("trace('Finished initializing LuaLoader.')");

  this->lua.new_usertype<Engine>("Engine", sol::no_constructor, "scene",
                                 &Engine::scene, "s", &Engine::scene,
                                 "SetScene", &Engine::SetScene);

  this->lua.new_usertype<Scene>(
      "Scene", sol::factories([]() { return std::make_shared<Scene>(); }),
      "AddGameObject", &Scene::AddGameObject, "RenameGameObject",
      sol::overload(static_cast<void (Scene::*)(const std::string &,
                                                const std::string &)>(
                        &Scene::RenameGameObject),
                    static_cast<void (Scene::*)(std::shared_ptr<GameObject>,
                                                const std::string &)>(
                        &Scene::RenameGameObject)),
      "RemoveGameObject",
      sol::overload(static_cast<void (Scene::*)(const std::string &)>(
                        &Scene::RemoveGameObject),
                    static_cast<void (Scene::*)(std::shared_ptr<GameObject>)>(
                        &Scene::RemoveGameObject)),
      "GetGameObject", &Scene::GetGameObject, "GetGameObjects",
      &Scene::GetGameObjectsVector, "GetGameObjectsPair",
      &Scene::GetGameObjectsPair,

      sol::meta_function::index, [](Scene &scene, const std::string &key) {
        return scene.GetGameObject(key);
      });

  this->lua.new_usertype<GameObject>(
      "GameObject",
      sol::factories([]() { return std::make_shared<GameObject>(); }),
      "transform", &GameObject::transform, "hasTexture",
      &GameObject::hasTexture, "textureHandle", &GameObject::textureHandle,
      "colour", &GameObject::colour, "SetPosition", &GameObject::SetPosition,
      "SetScale", &GameObject::SetScale, "SetRotation",
      &GameObject::SetRotation, "SetColour", &GameObject::SetColour,
      "Translate", &GameObject::Translate, "Scale", &GameObject::Scale,
      "Rotate", &GameObject::Rotate, "GetPosition", &GameObject::GetPosition,
      "GetScale", &GameObject::GetScale, "GetRotation",
      &GameObject::GetRotation, "GetColour", &GameObject::GetColour);

  lua.new_usertype<Scene::GameObjectPair>("GameObjectPair", sol::no_constructor,
                                          "name", &Scene::GameObjectPair::name,
                                          "obj", &Scene::GameObjectPair::obj);

  lua.new_usertype<glm::vec2>(
      "vec2", sol::constructors<glm::vec2(), glm::vec2(float, float)>(), "x",
      &glm::vec2::x, "y", &glm::vec2::y,

      sol::meta_function::addition,
      sol::overload(
          [](const glm::vec2 &lhs, const glm::vec2 &rhs) { return lhs + rhs; },
          [](const glm::vec2 &lhs, float rhs) { return lhs + rhs; },
          [](float lhs, const glm::vec2 &rhs) { return lhs + rhs; }),

      sol::meta_function::subtraction,
      sol::overload(
          [](const glm::vec2 &lhs, const glm::vec2 &rhs) { return lhs - rhs; },
          [](const glm::vec2 &lhs, float rhs) { return lhs - rhs; },
          [](float lhs, const glm::vec2 &rhs) { return lhs - rhs; }),

      sol::meta_function::multiplication,
      sol::overload(
          [](const glm::vec2 &lhs, const glm::vec2 &rhs) { return lhs * rhs; },
          [](const glm::vec2 &lhs, float rhs) { return lhs * rhs; },
          [](float lhs, const glm::vec2 &rhs) { return lhs * rhs; }),

      sol::meta_function::division,
      sol::overload(
          [](const glm::vec2 &lhs, const glm::vec2 &rhs) { return lhs / rhs; },
          [](const glm::vec2 &lhs, float rhs) { return lhs / rhs; },
          [](float lhs, const glm::vec2 &rhs) { return lhs / rhs; }),

      sol::meta_function::equal_to,
      sol::overload(
          [](const glm::vec2 &lhs, const glm::vec2 &rhs) { return lhs == rhs; },
          [](const glm::vec2 &lhs, float rhs) { return lhs == glm::vec2(rhs); },
          [](float lhs, const glm::vec2 &rhs) {
            return glm::vec2(lhs) == rhs;
          }),

      sol::meta_function::less_than,
      sol::overload(
          [](const glm::vec2 &lhs, const glm::vec2 &rhs) {
            return (lhs.x < rhs.x) && (lhs.y < rhs.y);
          },
          [](const glm::vec2 &lhs, float rhs) {
            return (lhs.x < rhs) && (lhs.y < rhs);
          },
          [](float lhs, const glm::vec2 &rhs) {
            return (lhs < rhs.x) && (lhs < rhs.y);
          }),

      sol::meta_function::less_than_or_equal_to,
      sol::overload(
          [](const glm::vec2 &lhs, const glm::vec2 &rhs) {
            return (lhs.x <= rhs.x) && (lhs.y <= rhs.y);
          },
          [](const glm::vec2 &lhs, float rhs) {
            return (lhs.x <= rhs) && (lhs.y <= rhs);
          },
          [](float lhs, const glm::vec2 &rhs) {
            return (lhs <= rhs.x) && (lhs <= rhs.y);
          }),

      sol::meta_function::to_string,
      [](const glm::vec2 &vec) {
        std::stringstream ss;
        ss << "vec2(" << vec.x << ", " << vec.y << ")";
        return ss.str();
      },

      sol::meta_function::index,
      [](const glm::vec2 &vec, const std::string &key) {
        if (key == "x") {
          return vec.x;
        } else if (key == "y") {
          return vec.y;
        } else if (key == "r") {
          return vec.x;
        } else if (key == "g") {
          return vec.y;
        } else if (key == "s") {
          return vec.x;
        } else if (key == "t") {
          return vec.y;
        } else {
          return -1.0f;
        }
      },

      "Distance",
      [](const glm::vec2 &lhs, const glm::vec2 &rhs) {
        return glm::distance(lhs, rhs);
      });

  // glm::vec3
  lua.new_usertype<glm::vec3>(
      "vec3", sol::constructors<glm::vec3(), glm::vec3(float, float, float)>(),
      "x", &glm::vec3::x, "y", &glm::vec3::y, "z", &glm::vec3::z,

      // Operators
      // +, -, *, /, ==, <, >, <=, >= are our targets
      sol::meta_function::addition,
      sol::overload(
          [](const glm::vec3 &lhs, const glm::vec3 &rhs) { return lhs + rhs; },
          [](const glm::vec3 &lhs, float rhs) { return lhs + rhs; },
          [](float lhs, const glm::vec3 &rhs) { return lhs + rhs; }),

      sol::meta_function::subtraction,
      sol::overload(
          [](const glm::vec3 &lhs, const glm::vec3 &rhs) { return lhs - rhs; },
          [](const glm::vec3 &lhs, float rhs) { return lhs - rhs; },
          [](float lhs, const glm::vec3 &rhs) { return lhs - rhs; }),

      sol::meta_function::multiplication,
      sol::overload(
          [](const glm::vec3 &lhs, const glm::vec3 &rhs) { return lhs * rhs; },
          [](const glm::vec3 &lhs, float rhs) { return lhs * rhs; },
          [](float lhs, const glm::vec3 &rhs) { return lhs * rhs; }),

      sol::meta_function::division,
      sol::overload(
          [](const glm::vec3 &lhs, const glm::vec3 &rhs) { return lhs / rhs; },
          [](const glm::vec3 &lhs, float rhs) { return lhs / rhs; },
          [](float lhs, const glm::vec3 &rhs) { return lhs / rhs; }),

      sol::meta_function::equal_to,
      sol::overload(
          [](const glm::vec3 &lhs, const glm::vec3 &rhs) { return lhs == rhs; },
          [](const glm::vec3 &lhs, float rhs) { return lhs == glm::vec3(rhs); },
          [](float lhs, const glm::vec3 &rhs) {
            return glm::vec3(lhs) == rhs;
          }),

      sol::meta_function::less_than,
      sol::overload(
          [](const glm::vec3 &lhs, const glm::vec3 &rhs) {
            return (lhs.x < rhs.x) && (lhs.y < rhs.y) && (lhs.z < rhs.z);
          },
          [](const glm::vec3 &lhs, float rhs) {
            return (lhs.x < rhs) && (lhs.y < rhs) && (lhs.z < rhs);
          },
          [](float lhs, const glm::vec3 &rhs) {
            return (lhs < rhs.x) && (lhs < rhs.y) && (lhs < rhs.z);
          }),

      sol::meta_function::less_than_or_equal_to,
      sol::overload(
          [](const glm::vec3 &lhs, const glm::vec3 &rhs) {
            return (lhs.x <= rhs.x) && (lhs.y <= rhs.y) && (lhs.z <= rhs.z);
          },
          [](const glm::vec3 &lhs, float rhs) {
            return (lhs.x <= rhs) && (lhs.y <= rhs) && (lhs.z <= rhs);
          },
          [](float lhs, const glm::vec3 &rhs) {
            return (lhs <= rhs.x) && (lhs <= rhs.y) && (lhs <= rhs.z);
          }),

      sol::meta_function::to_string,
      [](const glm::vec3 &vec) {
        std::stringstream ss;
        ss << "vec3(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
        return ss.str();
      },

      sol::meta_function::index,
      [](const glm::vec3 &vec, const std::string &key) {
        if (key == "x") {
          return vec.x;
        } else if (key == "y") {
          return vec.y;
        } else if (key == "z") {
          return vec.z;
        } else if (key == "r") {
          return vec.x;
        } else if (key == "g") {
          return vec.y;
        } else if (key == "b") {
          return vec.z;
        } else if (key == "s") {
          return vec.x;
        } else if (key == "t") {
          return vec.y;
        } else if (key == "p") {
          return vec.z;
        } else {
          return -1.0f;
        }
      },

      "Distance",
      [](const glm::vec3 &lhs, const glm::vec3 &rhs) {
        return glm::distance(lhs, rhs);
      });

  // Register the engine object
  this->lua.set("engine", this->engine);
  this->lua.set("e", this->engine);
}

void LuaLoader::OneShot(const std::string &script) {
  auto res = this->lua.safe_script(script, sol::script_pass_on_error);
  if (!res.valid()) {
    sol::error err = res;
    spdlog::error("LuaLoader::OneShot(): {}", err.what());
  }
}

void LuaLoader::LoadScript(const std::string &script) {
  auto res = this->lua.safe_script_file(script, sol::script_pass_on_error);
  if (!res.valid()) {
    sol::error err = res;
    spdlog::error("LuaLoader::LoadScript(): {}", err.what());
    return;
  }

  LoadedScript loadedScript;

  sol::optional<sol::function> ready = this->lua["ready"];
  sol::optional<sol::function> update = this->lua["update"];
  sol::optional<sol::function> draw = this->lua["draw"];

  loadedScript.ready = ready.has_value() ? ready.value() : sol::nil;
  loadedScript.update = update.has_value() ? update.value() : sol::nil;
  loadedScript.draw = draw.has_value() ? draw.value() : sol::nil;

  this->scripts.emplace_back(loadedScript);
}

void LuaLoader::LoadDirectory(const std::string &directory) {
  this->loadedDirectories.emplace_back(directory);

  try {
    for (const auto &entry : std::filesystem::directory_iterator(directory)) {
      if (entry.is_regular_file()) {
        this->LoadScript(entry.path().string());
      }
    }
  } catch (std::filesystem::filesystem_error &e) {
    spdlog::error("LuaLoader::LoadDirectory(): {}", e.what());
  }
}

void LuaLoader::Ready() {
  this->lua.set("scene", this->engine->scene);
  this->lua.set("s", this->engine->scene);
  this->lua.set("workspace", this->engine->scene);
  this->lua.set("ws", this->engine->scene);

  for (auto &script : this->scripts) {
    if (script.enabled && script.ready != sol::nil) {
      sol::safe_function_result res = script.ready();
      if (!res.valid()) {
        sol::error err = res;
        spdlog::error("LuaLoader::Ready(): {}", err.what());
      }
    }
  }
}

void LuaLoader::ReloadDirectories() {
  this->scripts.clear();

  std::vector<std::string> directories = this->loadedDirectories;
  this->loadedDirectories.clear();

  for (const auto &directory : directories) {
    this->LoadDirectory(directory);
  }

  this->Ready();
}

void LuaLoader::Update() {
  for (auto &script : this->scripts) {
    if (script.enabled && script.update != sol::nil) {
      auto res = script.update();

      if (!res.valid()) {
        sol::error err = res;
        spdlog::error(
            "{}{}",
            fmt::format(fmt::emphasis::bold | fmt::fg(fmt::color::light_blue),
                        "[LUA] "),
            err.what());
      }
    }
  }
}

void LuaLoader::Draw() {
  ImGuiErrorRecoveryState state;
  ImGui::ErrorRecoveryStoreState(&state);

  try {
    for (auto &script : this->scripts) {
      if (script.enabled && script.draw != sol::nil) {
        sol::safe_function_result res = script.draw();
        if (!res.valid()) {
          sol::error err = res;
          spdlog::error("LuaLoader::Draw(): {}", err.what());
        }
      }
    }
  } catch (...) {
    spdlog::error("LuaLoader::Draw(): An error occured, attempting to recover "
                  "ImGui state.");

    ImGui::ErrorRecoveryTryToRecoverState(&state);
  }
}

sol::state &LuaLoader::GetLua() { return this->lua; }
std::vector<LoadedScript> &LuaLoader::GetScripts() { return this->scripts; }

void LuaLoader::Cleanup() {
  spdlog::trace("LuaLoader::Cleanup()");

  this->lua.collect_garbage();
  this->scripts.clear();
}

// Utility function to construct a string from a variadic argument list.
std::string construct_string(sol::variadic_args va) {
  std::stringstream ss;

  int i = 0;
  for (auto v : va) {
    if (v.get_type() == sol::type::number) {
      ss << v.get<int>();
    } else if (v.get_type() == sol::type::string) {
      ss << v.get<std::string>();
    } else if (v.get_type() == sol::type::boolean) {
      ss << (v.get<bool>() ? "true" : "false");
    } else if (v.get_type() == sol::type::nil) {
      ss << "nil";
    } else if (v.get_type() == sol::type::userdata) {
      auto ud = v.get<sol::userdata>();
      if (ud.is<glm::vec2>()) {
        auto vec = ud.as<glm::vec2>();
        ss << "vec2(" << vec.x << ", " << vec.y << ")";
      } else if (ud.is<glm::vec3>()) {
        auto vec = ud.as<glm::vec3>();
        ss << "vec3(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
      } else {
        ss << "unknown userdata";
      }
    }

    // Add a comma if there are more arguments
    if (i++ < va.size() - 1) {
      ss << ", ";
    }
  }

  return ss.str();
};
