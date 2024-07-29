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

static bool isCollision(const int8_t *ptr, int s, int e, int stride) {
  bool ret(false);
  for (int i = s; i <= e; i++) {
    ret |= *ptr;
    ptr += stride;
  }
  return ret;
}

class Simulator {
public:
  Simulator() : rd(), gen(rd()), dis(-1.0, 1.0) {}
  struct RealBall {
    static double distance(const RealBall &a, const RealBall &b);
    double x;
    double y;
    double ux;
    double uy;
  };
  void init();
  void update(double dt);
  const RealBall &getBall(int idx) const { return balls[idx]; }
  double getMaxDistance() const { return maxDistance; }
  double getOriginX() const { return originX; }
  double getOriginY() const { return originY; }
  double getEnergy() const;

private:
  void ballReflect();
  void updateOrigin();

private:
  double minDistance = 0.0;
  double maxDistance = 0.0;
  double originX = 0.0;
  double originY = 0.0;
  double potentialEnergy = 0;
  std::random_device rd;
  std::mt19937 gen;
  std::uniform_real_distribution<> dis;
  std::array<RealBall, kBallNum> balls;
};

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

void Simulator::init() {
  for (auto &ball : balls) {
    // ball = Ball(x + kInitPosW, y, kR, 0, 0);
    // double x1 = x * kCosTheta - y * kSinTheta;
    // double y1 = x * kSinTheta + y * kCosTheta;
    // x = x1;
    // y = y1;
    ball = RealBall{.x = dis(gen), .y = dis(gen), .ux = dis(gen), .uy = dis(gen)};
  }
  minDistance = 0;
  maxDistance = 0;

  for (int i = 0; i < balls.size(); i++) {
    RealBall &curBall = balls[i];
    for (int j = i + 1; j < balls.size(); j++) {
      RealBall &other = balls[j];
      double d = RealBall::distance(curBall, other);

      maxDistance = std::max(d, maxDistance);

      if (minDistance == 0.0)
        minDistance = d;
      else
        minDistance = std::min(d, minDistance);
    }
  }
  updateOrigin();

  std::cout << "init state\n";
}

void Simulator::update(double dt) {
  // update ball position
  ballReflect();

  for (auto &ball : balls) {
    ball.x += ball.ux * dt;
    ball.y += ball.uy * dt;
  }

  minDistance = 0;
  maxDistance = 0;
  potentialEnergy = 0;
  for (int i = 0; i < balls.size(); i++) {
    RealBall &curBall = balls[i];
    for (int j = i + 1; j < balls.size(); j++) {
      RealBall &other = balls[j];
      double d = RealBall::distance(curBall, other);

      maxDistance = std::max(d, maxDistance);

      if (minDistance == 0.0)
        minDistance = d;
      else
        minDistance = std::min(d, minDistance);

      potentialEnergy -= G * m * (m / d);

      double a = G * (m / (d * d));
      double du = a * dt;
      double ex = (other.x - curBall.x) / d;
      double ey = (other.y - curBall.y) / d;
      curBall.ux += du * ex;
      curBall.uy += du * ey;

      other.ux -= du * ex;
      other.uy -= du * ey;
      std::cout << "x = " << curBall.x << "\ty = " << curBall.y << "\n";
      std::cout << "ux = " << curBall.ux << "\tuy = " << curBall.uy << "\n";
      std::cout << "x = " << other.x << "\ty = " << other.y << "\n";
      std::cout << "ux = " << other.ux << "\tuy = " << other.uy << "\n";
      std::cout << "G = " << G << "\n";
      std::cout << "m = " << m << "\n";
      std::cout << "d = " << d << "\n";
      std::cout << "a = " << a << "\n";
      std::cout << "dt = " << dt << "\n";
      std::cout << "du = " << du << "\n";
      std::cout << "|e| = " << ex * ex + ey * ey << "\tex = " << ex << "\tey = " << ey << "\n";
    }
  }
  updateOrigin();
  auto totalE = getEnergy();
  auto Ev = totalE - potentialEnergy;
  std::cout << "total energy: " << totalE << ", Ev: " << Ev << ", Ep: " << potentialEnergy << "\n";
}

void Simulator::ballReflect() {
  if (minDistance < 2 * kR)
    init();
}

void Simulator::updateOrigin() {
  originX = 0;
  originY = 0;
  for (const auto &ball : balls) {
    originX += ball.x;
    originY += ball.y;
  }
  originX /= kBallNum;
  originY /= kBallNum;
}

double Simulator::getEnergy() const {
  double ret;
  for (const auto &ball : balls) {
    ret += 0.5 * m * ball.ux * ball.ux;
    ret += 0.5 * m * ball.uy * ball.uy;
  }
  return ret + potentialEnergy;
}

static auto reg = GameRegistry::regist(static_cast<int>(Engine::Type::game1),
                                       []() { return new Game1; });
