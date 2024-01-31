#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>

#include <QtCore/QPoint>
#include <QtCore/QSet>

#include "Game/Engine/Engine.h"
#include "Game/Engine/Registry.h"
#include "Game/Engine/Item/Item.h"

class QPainter;

namespace {
static constexpr int nh = 12;
static constexpr int nw = 16;
static constexpr double brickRate = 1.0 / 3.0;

static bool isCollision(const int8_t *ptr, int s, int e, int stride) {
  bool ret(false);
  for (int i = s; i <= e; i++) {
    ret |= *ptr;
    ptr += stride;
  }
  return ret;
}

static int kR = 10;
static int kWidth = 60;
static int kHeight = 10;
static int kNW = std::ceil(2 * kR / kWidth);
static int kNH = std::ceil(2 * kR / kHeight);

class Game0 : public Engine {
public:
  Game0() = default;
  ~Game0() override = default;

  void init() override {
    boardU = 4;
    ballUY = 3;

    brickWidth = w() / nw;
    brickHeight = h() * brickRate / nh;

    ball = Ball(w() / 2 - kR, h() - kHeight - 2 * kR, kR, 0, ballUY);
    board = Board(w() / 2 - kWidth / 2, h() - kHeight, kWidth, kHeight, 0, 0);
    for (int i = 0; i < nh * nw; i++)
      bricks[i] = 1;
  }
  void resize() override {}
  void paint(QPainter &painter) override {
    Ball::setPaintParam(painter);
    ball.paint(painter);
    Brick::setPaintParam(painter);
    board.paint(painter);

    int posX = 0, posY = 0;
    Brick brick(posX, posY, brickWidth, brickHeight);
    auto *it = bricks.begin();
    for (int i = 0; i < nh; i++) {
      posX = 0;
      brick.moveLeft(posX);
      for (int j = 0; j < nw; j++) {
        if (*it) {
          brick.paint(painter);
        }
        posX += brickWidth;
        brick.moveLeft(posX);
        ++it;
      }
      posY += brickHeight;
      brick.moveTop(posY);
    }
  }
  void update() override { updateCompute(); }

private:
  void updateCompute();
  void updateBoardU();
  void boardBound();
  void ballBoundReflect();
  void ballBoardReflect();
  void ballBrickReflect();

private:
  int boardU;
  int ballUY;
  int brickWidth;
  int brickHeight;
  Ball ball;
  Board board;
  // matrix<int8_t>(12, 16)
  class BrickMatrix : public std::array<int8_t, nh * nw> {
  public:
    int8_t &operator()(int cIdx, int rIdx) { return at(cIdx * nw + rIdx); }
    const int8_t &operator()(int cIdx, int rIdx) const {
      return at(cIdx * nw + rIdx);
    }
  };
  BrickMatrix bricks;
};
} // namespace

void Game0::updateCompute() {
  // update ball position
  ball.update();

  // update board speed, then update board position
  // reset board when position out of limit
  updateBoardU();
  board.update();
  boardBound();

  // ball reflect at bound
  ballBoundReflect();

  // ball reflect ad board
  ballBoardReflect();

  // ball reflect at brick
  ballBrickReflect();
}

void Game0::updateBoardU() {
  board.rux() = 0;
  if (keyboard().contains(Qt::Key_Left))
    board.rux() = -boardU;
  if (keyboard().contains(Qt::Key_Right))
    board.rux() += boardU;
}

void Game0::boardBound() {
  if (board.left() < 0)
    board.moveTo(QPoint(0, board.top()));
  if (board.right() > w())
    board.moveTo(QPoint(w() - board.width(), board.top()));
}

void Game0::ballBoundReflect() {
  if (ball.right() >= w() || ball.left() <= 0)
    ball.rux() = -ball.ux();
  if (ball.top() <= 0)
    ball.ruy() = -ball.uy();
  if (ball.bottom() >= h()) {
    ball.rux() = 0;
    ball.ruy() = 0;
  }
}

void Game0::ballBoardReflect() {
  if (ball.bottom() < board.top())
    return;

  auto ballCentorX = ball.center().x();
  auto boardCentorX = board.center().x();
  double scale =
      double(ballCentorX - boardCentorX) / (board.width() / double(2));
  if (std::abs(scale) <= 1) {
    auto duX = ball.bottom() > h() ? 0 : boardU * scale;
    auto uX = (ball.ux() + duX);
    if (std::abs(uX) > boardU) {
      uX = uX * boardU / std::abs(uX);
    }
    ball.rux() = uX;
    ball.ruy() = -ball.uy();
  }
}

void Game0::ballBrickReflect() {
  if (h() * brickRate <= ball.top() || ball.top() <= 0)
    return;
  std::array<int, 4> ballPos;
  int top, bottom, left, right;
  top = ball.top() / brickHeight - 1;
  bottom = ball.top() / brickHeight - 1;
  left = ball.left() / brickWidth;
  right = ball.right() / brickWidth;

  bool topCollision(false), bottomCollision(false), leftCollision(false),
      rightCollision(false);

  int8_t *it;
  int limitTop = std::max(0, top), limitBottom = std::min(bottom, nh - 1),
      limitLeft = std::max(0, left), limitRight = std::min(right, nw - 1);

  if (bottom >= 0 && bottom < nh) {
    it = &bricks(bottom, limitLeft);
    bottomCollision = isCollision(it, limitLeft, limitRight, 1);
  }
  if (left >= 0 && left < nw) {
    it = &bricks(limitTop, left);
    leftCollision = isCollision(it, limitTop, limitBottom, nw);
  }
  if (right >= 0 && right < nw) {
    it = &bricks(limitTop, right);
    rightCollision = isCollision(it, limitTop, limitBottom, nw);
  }
  if (top >= 0 && top < nh) {
    it = &bricks(top, limitLeft);
    topCollision = isCollision(it, limitLeft, limitRight, 1);
  }

  if (topCollision || bottomCollision)
    ball.ruy() = -ball.uy();
  if (leftCollision || rightCollision)
    ball.rux() = -ball.ux();

  if (topCollision) {
    for (int i = limitLeft; i <= limitRight; i++) {
      bricks(limitTop, i) = 0;
    }
  }

  if (bottomCollision) {
    for (int i = limitLeft; i <= limitRight; i++) {
      bricks(limitBottom, i) = 0;
    }
  }

  if (leftCollision) {
    for (int i = limitTop; i <= limitBottom; i++) {
      bricks(i, limitLeft) = 0;
    }
  }

  if (rightCollision) {
    for (int i = limitTop; i <= limitBottom; i++) {
      bricks(i, limitRight) = 0;
    }
  }
}


static auto reg = GameRegistry::regist(static_cast<int>(Engine::Type::game0),
                                       []() { return new Game0; });
