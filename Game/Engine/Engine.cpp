#include <functional>

#include "Game/Engine/Engine.h"
#include "Game/Engine/Registry.h"

Engine *Engine::create(Engine::Type type) {
  auto fn = GameRegistry::find(static_cast<int>(type));
  if (fn)
    return fn();
  return nullptr;
}
