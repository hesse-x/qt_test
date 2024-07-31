#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <random>
#include <iostream>

#include <QtCore/QPoint>
#include <QtCore/QSet>

#include "Game/Engine/Engine.h"
#include "Game/Engine/Registry.h"
#include "Game/Engine/Item/Item.h"

class QPainter;

namespace {
static constexpr int kBallNum = 2;
static constexpr double kR = 0.1;
static constexpr double kTheta = 2 * M_PI / kBallNum;
static constexpr double kSinTheta = std::sin(kTheta);
static constexpr double kCosTheta = std::cos(kTheta);
static constexpr double m = 1;
static constexpr double G = 1;

class Game1 : public Engine {
public:
  Game1() = default;
  ~Game1() override = default;

  void init() override {
    sim.init();
  }
  void resize() override {}
  void paint(QPainter &painter) override {
    Ball::setPaintParam(painter);
    double d = sim.getMaxDistance() * 1.3;
    double curScale = std::min(double(std::min(w(), h())) / d, 200.0);
    scale = curScale;
//    if (scale == 0.0)
//      scale = curScale;
//    else
//      scale = std::min(scale, curScale);
    int paintR = scale * kR;
    paintR = std::max(paintR, 5);
    int originX = w() / 2;
    int originY = h() / 2;
    for (int i = 0; i < kBallNum; i++) {
      const auto &ball = sim.getBall(i);
      int x = originX + scale * (ball.x - sim.getOriginX());
      int y = originY + scale * (ball.y - sim.getOriginY());

      Ball b(x, y, paintR, 0, 0);
      b.paint(painter);
    }
  }
  void update() override { sim.update(double(dt()) / 1000); }
private:
  double scale = 0;
  Simulator sim;
};
} // namespace
double Simulator::RealBall::distance(const RealBall &a, const RealBall &b) {
  double dx = a.x - b.x;
  double dy = a.y - b.y;
  return std::sqrt(dx * dx + dy * dy);
}

static auto reg = GameRegistry::regist(static_cast<int>(Engine::Type::game1),
                                       []() { return new Game1; });
