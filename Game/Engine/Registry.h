#include <functional>

class Engine;
class GameRegistry {
public:
  using createFn = std::function<Engine *()>;
  static bool regist(int idx, createFn fn);
  static createFn find(int idx);
};
