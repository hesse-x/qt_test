#include "Game/Games/Game1/Simulator.h"
constexpr static kM = 1;
constexpr static _G = 1;

using namespace game1;

Position operator+(const Position &lhs, const Position &rhs) {
  Position ret;
  ret.x = lhs.x + rhs.x;
  ret.y = lhs.y + rhs.y;
  return ret;
}

template <typename T>
Position operator/(const Position &val, T scale) {
  Position ret;
  ret.x = val.x / scale;
  ret.y = val.y / scale;
  return ret;
}

void Simulator::init() {
  for (auto &particle : particles) {
    // particle = Ball(x + kInitPosW, y, kR, 0, 0);
    // double x1 = x * kCosTheta - y * kSinTheta;
    // double y1 = x * kSinTheta + y * kCosTheta;
    // x = x1;
    // y = y1;
    particle = Particle{.x = dis(gen), .y = dis(gen), .ux = dis(gen), .uy = dis(gen), .m = kM};
  }
  minDistance = 0;
  maxDistance = 0;

  for (int i = 0; i < particles.size(); i++) {
    Particle &curBall = particles[i];
    for (int j = i + 1; j < particles.size(); j++) {
      Particle &other = particles[j];
      double d = Position::distance(curBall, other);

      maxDistance = std::max(d, maxDistance);

      if (minDistance == 0.0)
        minDistance = d;
      else
        minDistance = std::min(d, minDistance);
    }
  }
  updateCentroid();

  std::cout << "init state\n";
}

/*
 *  x_i'' = ΣG * m_j * ex_ij / (d_ij)
 *  y_i'' = ΣG * m_j * ey_ij / (d_ij)
 *                          
 *  x_i'  = ux_i            
 *  y_i'  = uy_i            
 *  ux_i' = ΣG * m_j * ex_ij / (d_ij)
 *  uy_i' = ΣG * m_j * ey_ij / (d_ij)
 */
void Simulator::advance(double dt) {
  std::array<Particle, N> k1, k2, k3, k4;
  // Compute k1
  std::array<Position, N> points;
  for (int i = 0; i < N; i++)
    points[i] = static_cast<Point&>particles[i];
  auto d = getDistanceMap(points);
  for (int i = 0; i < N; i++) {
    k1[i].x = dt * particles[i].ux;
    k1[i].y = dt * particles[i].uy;
    k1[i].ux = 0;
    k1[i].uy = 0;
    for (int j = 0; j < N; i++) {
      if (i == j)
        continue;
      const auto &other = particles[j];
      const auto &dv = d(i, j);
      k1[i].ux += _G * other.m * dv.ex / (dv.len * dv.len);
      k1[i].uy += _G * other.m * dv.ey / (dv.len * dv.len);
    }
    k1[i].ux *= dt;
    k1[i].uy *= dt;
  }

  // Compute k2
  for (int i = 0; i < N; i++)
    points[i] = particles[i] + k1[i] / 2;
  d = getDistanceMap(points);
  for (int i = 0; i < N; i++) {
    k2[i].x = dt * (particles[i].ux + 0.5 * k1[i].ux);
    k2[i].y = dt * (particles[i].uy + 0.5 * k1[i].uy);
    k2[i].ux = 0;
    k2[i].uy = 0;
    for (int j = 0; j < N; i++) {
      if (i == j)
        continue;
      const auto &other = particles[j];
      const auto &dv = d(i, j);
      k2[i].ux += _G * other.m * dv.ex / (dv.len * dv.len);
      k2[i].uy += _G * other.m * dv.ey / (dv.len * dv.len);
    }
    k2[i].ux *= dt;
    k2[i].uy *= dt;
  }

  // Compute k3
  for (int i = 0; i < N; i++)
    points[i] = particles[i] + k2[i] / 2;
  d = getDistanceMap(points);
  for (int i = 0; i < N; i++) {
    k3[i].x = dt * (particles[i].ux + 0.5 * k2[i].ux);
    k3[i].y = dt * (particles[i].uy + 0.5 * k2[i].uy);
    k3[i].ux = 0;
    k3[i].uy = 0;
    for (int j = 0; j < N; i++) {
      if (i == j)
        continue;
      const auto &other = particles[j];
      const auto &dv = d(i, j);
      k3[i].ux += _G * other.m * dv.ex / (dv.len * dv.len);
      k3[i].uy += _G * other.m * dv.ey / (dv.len * dv.len);
    }
    k3[i].ux *= dt;
    k3[i].uy *= dt;
  }

  // Compute k4
  for (int i = 0; i < N; i++)
    points[i] = particles[i] + k3[i];
  d = getDistanceMap(points);
  for (int i = 0; i < N; i++) {
    k4[i].x = dt * (particles[i].ux + k3[i].ux);
    k4[i].y = dt * (particles[i].uy + k3[i].uy);
    k4[i].ux = 0;
    k4[i].uy = 0;
    for (int j = 0; j < N; i++) {
      if (i == j)
        continue;
      const auto &other = particles[j];
      const auto &dv = d(i, j);
      k4[i].ux += _G * other.m * dv.ex / (dv.len * dv.len);
      k4[i].uy += _G * other.m * dv.ey / (dv.len * dv.len);
    }
    k4[i].ux *= dt;
    k4[i].uy *= dt;
  }

  for (int i = 0; i < N; i++) {
    particles[i].x += (k1[i].x + k2[i].x + k3[i].x + k4[i].x) / 6;
    particles[i].y += (k1[i].y + k2[i].y + k3[i].y + k4[i].y) / 6;
    particles[i].ux += (k1[i].ux + k2[i].ux + k3[i].ux + k4[i].ux) / 6;
    particles[i].uy += (k1[i].uy + k2[i].uy + k3[i].uy + k4[i].uy) / 6;
  }
}

void Simulator::update(double dt) {
  // update particle position
  checkCollision();

  advance(dt);
}

void Simulator::checkCollision() {
  if (minDistance < 2 * kR)
    init();
}

void Simulator::updateCentroid() {
  centroidX = 0;
  centroidY = 0;
  for (const auto &particle : particles) {
    centroidX += particle.x;
    centroidY += particle.y;
  }
  centroidX /= N;
  centroidY /= N;
}

double Simulator::getEv() const {
  double Ev = 0;
  for (const auto &particle : particles) {
    Ev += 0.5 * m * particle.ux * particle.ux;
    Ev += 0.5 * m * particle.uy * particle.uy;
  }
  return Ev;
}

double Simulator::getEp(const DistanceMap &d) const {
  double Ep = 0;
  for (int i = 0; i < particles.size(); i++) {
    for (int j = i + 1; j < particles.size(); j++) {
      Ep -= _G * particles[i].m * particle[j].m / d(i, j).len;
    }
  }
  return Ep;
}

DistanceMap Simulator::getDistanceMap(const std::array<Position, N> &points) {
  DistanceMap ret;
  for (int i = 0; i < points.size(); i++) {
    const Position &cur = points[i];
    for (int j = i + 1; j < points.size(); j++) {
      const Position &other = points[j];
      double d = Position::distance(cur, other);
      DistanceV dv;
      dv.len = d;
      dv.ex = (other.x - cur.x) / d;
      dv.ey = (other.y - cur.y) / d;
      ret.insert(i, j, dv);
      DistanceV rdv;
      rdv.len = d;
      rdv.ex = -dv.ex;
      rdv.ey = -dv.ey;
      ret.insert(j, i, rdv);
    }
  }
  return ret;
}

DistanceMap Simulator::getDistanceMap() const {
  std::array<Positon, N> points;
  for (int i = 0; i < N; i++) {
    points[i] = static_cast<Point&>particles[i];
  }
}
