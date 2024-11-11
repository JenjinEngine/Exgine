#include "exgine/scene.h"

#include <spdlog/spdlog.h>

using namespace Exgine;

Scene::Scene() { spdlog::trace("Scene::Scene()"); }

void Scene::AddGameObject(const std::string &name,
                          std::shared_ptr<GameObject> gameObject) {
  gameObjects[name] = gameObject;
}

void Scene::RenameGameObject(const std::string &oldName,
                             const std::string &newName) {
  gameObjects[newName] = gameObjects[oldName];
  gameObjects.erase(oldName);
}

void Scene::RenameGameObject(std::shared_ptr<GameObject> gameObject,
                             const std::string &newName) {
  for (auto &[name, obj] : gameObjects) {
    if (obj == gameObject) {
      gameObjects[newName] = obj;
      gameObjects.erase(name);
      return;
    }
  }
}

void Scene::RemoveGameObject(const std::string &name) {
  gameObjects.erase(name);
}

void Scene::RemoveGameObject(std::shared_ptr<GameObject> gameObject) {
  for (auto &[name, obj] : gameObjects) {
    if (obj == gameObject) {
      gameObjects.erase(name);
      return;
    }
  }
}

Scene::~Scene() { spdlog::trace("Scene::~Scene()"); }
