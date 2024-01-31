#include <optional>

class QPoint;
template <class T> class QSet;
class QPainter;

class Engine {
public:
  Engine() = default;
  virtual ~Engine() = default;
  enum class Type : int {
    game0 = 0,
  };
  static Engine *create(Type type);
  void init(int w, int h, int dt, QSet<int> *keyboard,
            std::optional<QPoint> *mouse) {
    windowWidth = w;
    windowHeight = h;
    timeInterval = dt;
    mousePos = mouse;
    keyboardState = keyboard;
    init();
  }
  void resize(int w, int h) {
    windowWidth = w;
    windowHeight = h;
    resize();
  }
  virtual void paint(QPainter &painter) = 0;
  virtual void update() = 0;

protected:
  int w() const { return windowWidth; }
  int h() const { return windowHeight; }
  int dt() const { return timeInterval; }
  const std::optional<QPoint> &mouse() const { return *mousePos; }
  const QSet<int> &keyboard() const { return *keyboardState; }
  virtual void resize() = 0;
  virtual void init() = 0;

private:
  int windowWidth;
  int windowHeight;
  int timeInterval;
  std::optional<QPoint> *mousePos;
  QSet<int> *keyboardState;
};
