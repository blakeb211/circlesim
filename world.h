#pragma once
#include "actor.h"
#include <cstdlib>
#include <vector>

class World {
  size_t DIMX;

public:
  std::string name{};
  size_t dimx() const { return DIMX; }
  ~World() {
    for (auto o : objs) {
      o->~Actor();
    }
  };
  World() = delete;
  World(int id) : DIMX{25} {
    switch (id) {
    case 42:
      // create hero character first
      objs.push_back(new Actor(
          v2{static_cast<float>(DIMX / 2), static_cast<float>(DIMX - 1)},
          PI / 2, nullptr, Shape::QUAD));
      for (int x = 0; x < DIMX; x++) {
        for (int y = 0; y < DIMX; y++) {
          // sparse entity generation for this particular world
          float ang = -90 * DEG2RAD;
          if (rand() % 10 > 7)
            objs.push_back(
                new Actor(v2{static_cast<float>(x), static_cast<float>(y)}, ang,
                          new RandomMoveState{}));
        }
      }
      break;
    default:
      assert(0);
    };
  };
  bool pos_valid(v2 pos) {
    // Check if cell is occupied already.
    // We are on a unit grid.
    {
      v2 dR{};
      for (auto o : objs) {
        dR = o->pos - pos;
        dR.x = std::abs(dR.x);
        dR.y = std::abs(dR.y);
        if (dR.x < 1 && dR.y < 1) {
          return false;
        }
      }
    }
    return (pos.x <= DIMX - 1) && (pos.x >= 0) && (pos.y <= DIMX - 1) &&
           (pos.y >= 0);
  }
  std::vector<Actor *> objs;
};