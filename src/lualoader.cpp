#define SOL_ALL_SAFETIES_ON 1
#define GLFW_INCLUDE_NONE

#include <exgine/lualoader.h>

#include <GLFW/glfw3.h>
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

  igt["ContentRegion"] = &ImGui::GetContentRegionAvail;

  igt["GetIO"] = &ImGui::GetIO;

  lua.new_usertype<ImVec2>("ImVec2", sol::constructors<float, float>(), "x",
                           &ImVec2::x, "y", &ImVec2::y);

  lua.new_usertype<ImVec4>(
      "ImVec4", sol::constructors<float, float, float, float>(), "x",
      &ImVec4::x, "y", &ImVec4::y, "z", &ImVec4::z, "w", &ImVec4::w);

  lua.new_usertype<ImGuiIO>(
      "ImGuiIO", sol::no_constructor, "MousePos", &ImGuiIO::MousePos,
      "MouseDown", &ImGuiIO::MouseDown, "MouseWheel", &ImGuiIO::MouseWheel,
      "MouseWheelH", &ImGuiIO::MouseWheelH, "MouseWheel", &ImGuiIO::MouseWheel,
      "DeltaTime", &ImGuiIO::DeltaTime, "KeyCtrl", &ImGuiIO::KeyCtrl,
      "KeyShift", &ImGuiIO::KeyShift, "KeyAlt", &ImGuiIO::KeyAlt, "KeySuper",
      &ImGuiIO::KeySuper, "KeysDown", &ImGuiIO::KeysDown, "NavInputs",
      &ImGuiIO::NavInputs, "WantCaptureMouse", &ImGuiIO::WantCaptureMouse,
      "WantCaptureKeyboard", &ImGuiIO::WantCaptureKeyboard, "WantTextInput",
      &ImGuiIO::WantTextInput, "WantSetMousePos", &ImGuiIO::WantSetMousePos,
      "WantSaveIniSettings", &ImGuiIO::WantSaveIniSettings, "NavActive",
      &ImGuiIO::NavActive, "NavVisible", &ImGuiIO::NavVisible, "Framerate",
      &ImGuiIO::Framerate);

  igt["ImVec2"] = [](float x, float y) { return ImVec2(x, y); };
  igt["ImVec4"] = [](float x, float y, float z, float w) {
    return ImVec4(x, y, z, w);
  };

  igt["Image"] = [](int user_texture_id, const ImVec2 &size) {
    ImGui::Image((ImTextureID)user_texture_id, size);
  };

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

  this->lua.new_usertype<Engine>(
      "Engine", sol::no_constructor, "scene", &Engine::scene, "s",
      &Engine::scene, "SetScene", &Engine::SetScene, "luaLoader",
      &Engine::luaLoader, "framebuffer", &Engine::framebuffer, "renderer",
      &Engine::renderer);

  this->lua.new_usertype<Framebuffer>(
      "Framebuffer", sol::no_constructor, "Bind", &Framebuffer::Bind, "Resize",
      &Framebuffer::Resize, "Unbind", &Framebuffer::Unbind, "texture",
      &Framebuffer::texture);

  this->lua.new_usertype<Renderer>(
      "Renderer", sol::no_constructor, "manualResize", &Renderer::manualResize,
      "width", &Renderer::width, "height", &Renderer::height);

  this->lua.new_usertype<LuaLoader>(
      "LuaLoader", sol::no_constructor, "ReloadDirectories",
      &LuaLoader::ReloadDirectories, "Reload", &LuaLoader::ReloadDirectories);

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
      &glm::vec2::x, "y", &glm::vec2::y, "w", &glm::vec2::x, "h", &glm::vec2::y,

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
      "x", &glm::vec3::x, "y", &glm::vec3::y, "z", &glm::vec3::z, "w",
      &glm::vec3::x, "h", &glm::vec3::y, "d", &glm::vec3::z,

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

  auto inputTable = this->lua.create_named_table("input");
  inputTable.set_function("IsKeyDown", [&](const std::string &key) {
    static std::unordered_map<std::string, int> keys = {};

    if (keys.empty()) {
      spdlog::info("[LUA] Initializing key map");

      for (int i = 0; i < GLFW_KEY_LAST; i++) {
        auto keyName = glfwGetKeyName(i, 0);
        if (keyName) {
          keys[keyName] = i;
        }
      }
    }

    auto context = glfwGetCurrentContext();

    if (context) {
      std::string key_lower = key;
      std::transform(key_lower.begin(), key_lower.end(), key_lower.begin(),
                     [](unsigned char c) { return std::tolower(c); });

      return glfwGetKey(context, keys[key_lower]) == GLFW_PRESS;
    } else {
      spdlog::error("[LUA] No current context while calling `GetKeyDown`");
      return false;
    }
  });

  inputTable.set_function("IsMouseButtonDown", [&](int button) {
    return glfwGetMouseButton(glfwGetCurrentContext(), button) == GLFW_PRESS;
  });

  // NOTE: has to get in NDC
  inputTable.set_function("GetMousePosition", [&]() {
    double x, y;
    glfwGetCursorPos(glfwGetCurrentContext(), &x, &y);

    int w, h;
    glfwGetWindowSize(glfwGetCurrentContext(), &w, &h);

    // Convert to NDC
    x = (x / w) * 2.0f - 1.0f;
    y = (y / h) * 2.0f - 1.0f;

    return glm::vec2(x, y);
  });

  lua.set("LMB", GLFW_MOUSE_BUTTON_LEFT);
  lua.set("RMB", GLFW_MOUSE_BUTTON_RIGHT);
  lua.set("MMB", GLFW_MOUSE_BUTTON_MIDDLE);

  auto utilsTable = this->lua.create_named_table("utils");

  utilsTable.set_function(
      "clearColor",
      [&](float r, float g, float b, float a) { glClearColor(r, g, b, a); });

  utilsTable.set_function("clear", [&]() { glClear(GL_COLOR_BUFFER_BIT); });

  utilsTable.set_function("getWindowSize", [&]() {
    int w, h;
    glfwGetWindowSize(glfwGetCurrentContext(), &w, &h);
    return glm::vec2(w, h);
  });

  static bool vsync = true; // VSync is enabled by default
  utilsTable.set_function("SetVSync", [&](bool enabled) {
    glfwSwapInterval(enabled ? 1 : 0);
    vsync = enabled;
  });

  utilsTable.set_function("GetVSync", [&]() { return vsync; });

  utilsTable.set_function("ToggleVSync", [&]() {
    glfwSwapInterval(vsync ? 0 : 1);
    vsync = !vsync;
  });

  utilsTable.set_function("ReloadLua", [&]() { this->ReloadDirectories(); });
  utilsTable.set_function("GetTime", [&]() { return glfwGetTime(); });
}

void LuaLoader::OneShot(const std::string &script) {
  auto res = this->lua.safe_script(script, sol::script_pass_on_error);
  if (!res.valid()) {
    sol::error err = res;
    spdlog::error("LuaLoader::OneShot(): {}", err.what());
  }
}

void LuaLoader::LoadScript(const std::string &script) {
  // Clear out, preDraw, ready, update, draw, postDraw, etc.
  // which can potentially be left over from a previous script.
  for (auto &ident : {"preDraw", "ready", "update", "draw", "postDraw"}) {
    this->lua.set(ident, sol::nil);
  }

  auto res = this->lua.safe_script_file(script, sol::script_pass_on_error);
  if (!res.valid()) {
    sol::error err = res;
    spdlog::error("LuaLoader::LoadScript(): {}", err.what());
    return;
  }

  LoadedScript loadedScript;

  /*loadedScript.priority = lua["PRIORITY"].get_or(0);*/
  /*loadedScript.preDraw = lua["preDraw"].get_or(sol::nil);*/
  /*loadedScript.ready = lua["ready"].get_or(sol::nil);*/
  /*loadedScript.update = lua["update"].get_or(sol::nil);*/
  /*loadedScript.draw = lua["draw"].get_or(sol::nil);*/
  /*loadedScript.postDraw = lua["postDraw"].get_or(sol::nil);*/

  sol::optional<sol::function> preDraw = this->lua["preDraw"];
  sol::optional<sol::function> ready = this->lua["ready"];
  sol::optional<sol::function> update = this->lua["update"];
  sol::optional<sol::function> draw = this->lua["draw"];
  sol::optional<sol::function> postDraw = this->lua["postDraw"];

  loadedScript.priority = this->lua["PRIORITY"].get_or(0);
  loadedScript.preDraw = preDraw.has_value() ? preDraw.value() : sol::nil;
  loadedScript.ready = ready.has_value() ? ready.value() : sol::nil;
  loadedScript.update = update.has_value() ? update.value() : sol::nil;
  loadedScript.draw = draw.has_value() ? draw.value() : sol::nil;
  loadedScript.postDraw = postDraw.has_value() ? postDraw.value() : sol::nil;

  this->scripts.emplace_back(loadedScript);
}

void LuaLoader::LoadDirectory(const std::string &directory, bool saveToList) {
  if (saveToList)
    this->loadedDirectories.emplace_back(directory);

  try {
    for (const auto &entry : std::filesystem::directory_iterator(directory)) {
      if (entry.is_regular_file()) {
        this->LoadScript(entry.path().string());
      }

      if (entry.is_directory()) {
        this->LoadDirectory(entry.path().string(), false);
      }
    }
  } catch (std::filesystem::filesystem_error &e) {
    spdlog::error("LuaLoader::LoadDirectory(): {}", e.what());
  }

  // Sort the scripts by priority. (Low to high)
  std::sort(this->scripts.begin(), this->scripts.end(),
            [](const LoadedScript &lhs, const LoadedScript &rhs) {
              return lhs.priority < rhs.priority;
            });
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
  auto dt = ImGui::GetIO().DeltaTime;
  this->lua.set("deltaTime", dt);
  this->lua.set("dt", dt);

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

void LuaLoader::PreDraw() {
  ImGuiErrorRecoveryState state;
  ImGui::ErrorRecoveryStoreState(&state);

  try {
    for (auto &script : this->scripts) {
      if (script.enabled && script.preDraw != sol::nil) {
        sol::safe_function_result res = script.preDraw();
        if (!res.valid()) {
          sol::error err = res;
          spdlog::error("LuaLoader::PreDraw(): {}", err.what());
        }
      }
    }
  } catch (...) {
    spdlog::error("LuaLoader::PreDraw(): An error occurred, attempting to "
                  "recover ImGui state.");

    ImGui::ErrorRecoveryTryToRecoverState(&state);
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
    spdlog::error("LuaLoader::Draw(): An error occurred, attempting to recover "
                  "ImGui state.");

    ImGui::ErrorRecoveryTryToRecoverState(&state);
  }
}

void LuaLoader::PostDraw() {
  ImGuiErrorRecoveryState state;
  ImGui::ErrorRecoveryStoreState(&state);

  try {
    for (auto &script : this->scripts) {
      if (script.enabled && script.postDraw != sol::nil) {
        sol::safe_function_result res = script.postDraw();
        if (!res.valid()) {
          sol::error err = res;
          spdlog::error("LuaLoader::PostDraw(): {}", err.what());
        }
      }
    }
  } catch (...) {
    spdlog::error("LuaLoader::PostDraw(): An error occurred, attempting to "
                  "recover ImGui state.");

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
