#include <map>
#include <array>
#include <cstdint>
#include <random>
namespace geme1 {
class DistanceMap;

struct Position {
  static double distance(const Particle &a, const Particle &b);
  double x;
  double y;
};

struct Particle : public Position {
  double ux;
  double uy;
  double m;
};

struct DistanceV {
  double len;
  double ex;
  double ey;
};

template <int N>
class Simulator {
public:
  Simulator() : rd(), gen(rd()), dis(-1.0, 1.0) {}

  void init();
  void update(double dt);
  const Particle &getParticle(int idx) const { return particles[idx]; }
  double getMaxDistance() const { return maxDistance; }
  double getCentroidX() const { return centroidX; }
  double getCentroidY() const { return centroidY; }
  double getEv() const;
  double getEp(const DistanceMap &d) const;
  DistanceMap getDistanceMap() const;

private:
  static DistanceMap getDistanceMap(const std::array<Position, N> &points);
  void checkCollision();
  void updateCentroid();

private:
  double minDistance = 0.0;
  double maxDistance = 0.0;
  double centroidX = 0.0;
  double centroidY = 0.0;
  std::random_device rd;
  std::mt19937 gen;
  std::uniform_real_distribution<> dis;
  std::array<Particle, N> particles;
};

class DistanceMap {
public:
  void insert(int i, int j, DistanceV d) { map.insert({getKey(i, j), d}); }
  const DistanceV &operator()(int i, int j) const { return map.find(getKey(i, j))->second; }
  double max() const {
    double ret = 0;
    for (const auto &it : map) {
      ret = std::max(ret, it.second.len);
    }
    return ret;
  }
  double min() const {
    double ret = 0;
    for (const auto &it : map) {
      if (ret == 0)
        ret = it.second.len;
      else
        ret = std::max(ret, it.second.len);
    }
    return ret;
  }
private:
  static uint64_t getKey(int i, int j) {
    return (uint64_t(i) << 32) | j;
  }
  std::map<uint64_t, double> map;
};
} // namespace game1
