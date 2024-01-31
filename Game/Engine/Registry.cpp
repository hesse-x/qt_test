#include "Game/Engine/Registry.h"
#include <cassert>
#include <utility>

#include <unordered_map>
namespace {
class Registry {
public:
  using createFn = GameRegistry::createFn;
  bool regist(int idx, createFn fn) {
    assert(map.find(idx) == map.end());
    map[idx] = std::move(fn);
    return true;
  }
  createFn find(int idx) const {
    auto it = map.find(idx);
    if (it != map.end())
      return it->second;
    return nullptr;
  }

private:
  std::unordered_map<int, createFn> map;
};
} // namespace

static Registry *get() {
  static Registry reg;
  return &reg;
}

bool GameRegistry::regist(int idx, GameRegistry::createFn fn) {
  return get()->regist(idx, std::move(fn));
}

GameRegistry::createFn GameRegistry::find(int idx) { return get()->find(idx); }
