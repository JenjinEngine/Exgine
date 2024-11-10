#include "exgine/engine.h"
#include <numeric>
#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <spdlog/spdlog.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

#ifndef NDEBUG
  spdlog::set_level(spdlog::level::critical);
#endif

  if (argc > 1) {
    if (std::string(argv[1]) == "--debug") {
      spdlog::set_level(spdlog::level::trace);
    }
  } else {
    spdlog::set_level(spdlog::level::info);
  }

  if (!glfwInit()) {
    const char *error;
    int code = glfwGetError(&error);
    spdlog::critical("Failed to initialize GLFW: {} ({})", error, code);

    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

  // MSAA
  glfwWindowHint(GLFW_SAMPLES, 4);

  GLFWwindow *window = glfwCreateWindow(800, 600, "Exgine", nullptr, nullptr);

  if (!window) {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  /*glfwSwapInterval(0);*/

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    spdlog::critical("Failed to initialize GLAD");
    glfwTerminate();
    return -1;
  }

  Exgine::Engine engine(window);

  auto scene = std::make_shared<Exgine::Scene>();

  auto gobj = std::make_shared<Exgine::GameObject>();
  gobj->transform.scale = glm::vec2(10.0f, 1.0f);
  gobj->transform.position = glm::vec3(5.0f, 5.0f, 0.0f);
  gobj->colour = glm::vec3(0.8f, 0.8, 0.8f);
  gobj->transform.rotation = 45.0f;

  scene->AddGameObject("test", gobj);
  engine.SetScene(scene, true);

  engine.luaLoader->LoadDirectory("demo/scripts");
  engine.luaLoader->Ready();

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;

  spdlog::trace("Setup ImGui {}", IMGUI_VERSION);

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 460");

  while (!glfwWindowShouldClose(window) &&
         !glfwGetKey(window, GLFW_KEY_ESCAPE)) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    /*if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS)*/
    // TEMP: Debugging, reload all scripts constantly (there isn't enough of a performance hit
    // to warrant a more complex solution) (200-300µs, 16ms = 16000µs, this means reloading is 1.25% of the frame time, which is acceptable for demo)
    engine.luaLoader->ReloadDirectories();

    engine.luaLoader->Update();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    engine.Draw();
    engine.luaLoader->Draw();

    auto fps_text = fmt::format("FPS: {:.3}", ImGui::GetIO().Framerate);

    static float dts[1000] = {0};
    static int dti = 0;
    static int len = sizeof(dts) / sizeof(dts[0]);

    if (dts[0] == 0)
      std::fill(dts, dts + len, ImGui::GetIO().DeltaTime);

    dts[dti++ % len] = ImGui::GetIO().DeltaTime;
    auto dt_avg = std::accumulate(dts, dts + len, 0.0f) / len;

    ImGui::SetNextWindowPos(ImVec2(32, 32));

    ImGui::Begin("Stats", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoCollapse);

    if (ImGui::BeginTable("##statstab", 2)) {
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::Text("FPS");
      ImGui::TableNextColumn();
      ImGui::Text("%f", ImGui::GetIO().Framerate);

      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::Text("FPS AVG");
      ImGui::TableNextColumn();
      ImGui::Text("%f", 1.0f / dt_avg);

      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::Text("DT (µs)");
      ImGui::TableNextColumn();
      ImGui::Text("%f", ImGui::GetIO().DeltaTime * 1000000);

      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::Text("DT AVG (µs)");
      ImGui::TableNextColumn();
      ImGui::Text("%f", dt_avg * 1000000);

      ImGui::EndTable();
    }

    /*ImGui::Text(fps_text.c_str());*/
    /*ImGui::Text(fps_avg_text.c_str());*/
    /*ImGui::Text(dt_text.c_str());*/
    /*ImGui::Text(dt_avg_text.c_str());*/

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  engine.Cleanup();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
