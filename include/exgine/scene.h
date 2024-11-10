#pragma once

#define SOL_ALL_SAFETIES_ON 1
#define GLFW_INCLUDE_NONE

#include "exgine/gameobject.h"

#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>

namespace Exgine {
class Scene {
public:
  Scene();
  ~Scene();

  // Add a game object to the scene
  void AddGameObject(const std::string &name,
                     std::shared_ptr<GameObject> gameObject);

  // Rename a game object by name
  void RenameGameObject(const std::string &oldName, const std::string &newName);

  // Rename a game object by shared pointer
  void RenameGameObject(std::shared_ptr<GameObject> gameObject,
                        const std::string &newName);

  // Remove a game object from the scene by name
  void RemoveGameObject(const std::string &name);

  // Remove a game object from the scene by shared pointer
  void RemoveGameObject(std::shared_ptr<GameObject> gameObject);

  // Get a game object by name
  std::shared_ptr<GameObject> GetGameObject(const std::string &name) {
    return gameObjects[name];
  }

  std::unordered_map<std::string, std::shared_ptr<GameObject>> &
  GetGameObjects() {
    return gameObjects;
  }

  std::vector<std::shared_ptr<GameObject>> GetGameObjectsVector() {
    std::vector<std::shared_ptr<GameObject>> gameObjectsVector;
    gameObjectsVector.reserve(
        gameObjects.size()); // Reserve memory to avoid reallocations

    std::transform(gameObjects.begin(), gameObjects.end(),
                   std::back_inserter(gameObjectsVector),
                   [](const auto &pair) { return pair.second; });

    return gameObjectsVector;
  }

  // TODO: Different DS for hierarchy support
  // Get a game object by shared pointer
  std::unordered_map<std::string, std::shared_ptr<GameObject>> gameObjects;
};
} // namespace Exgine
