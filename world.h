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
  World(int id) : DIMX{40} {
    auto my_rand = rint_distr(1,100);
    switch (id) {
    case 42:
      // create hero character first
      objs.push_back(new Actor(
          v2{static_cast<float>(DIMX / 2), static_cast<float>(DIMX - 1)}, 0.f,
          nullptr, Shape::QUAD));
        
      for (int x = 0; x < DIMX; x++) {
        for (int y = 0; y < DIMX; y++) {
          // sparse entity generation for this particular world
          float ang = -90 * DEG2RAD;
          if (my_rand(generator) > 97) {
            objs.push_back(
                new Actor(v2{static_cast<float>(x), static_cast<float>(y)}, ang,
                          new RandomMoveState{}, Shape::CIRC));
          }
          else if (my_rand(generator) == 1) {
            objs.push_back(
                new Actor(v2{static_cast<float>(x), static_cast<float>(y)}, ang,
                          new RandomMoveState{}, Shape::THREEHAT));
          }
        } // y loop
      }   // x loop
      break;
    default:
      assert(0);
    };
  };
  bool pos_valid(Actor *a, v2 pos) {
    // We are on a unit grid.
    // Check if cell is occupied already.
    {
      v2 dR{};
      for (auto o : objs) {
        // skip checking object for collision with itself
        if (o == a)
          continue;
        dR = o->pos - pos;
        dR.x = std::abs(dR.x);
        dR.y = std::abs(dR.y);
        if (dR.x < 1 && dR.y < 1) {
          return false;
        }
        const auto sz_periph = o->periph.size();
        for (int i = 0; i < sz_periph; i++) {
          if (!o->visible[i])
            continue;
          dR = o->pos + o->periph[i] - pos;
          dR.x = std::abs(dR.x);
          dR.y = std::abs(dR.y);
          if (dR.x < 1 && dR.y < 1) {
            return false;
          }
        }
      }
    }
    // If we are here, no other actor blocks the move. Return whether it is
    // in range of the grid or not.
    return (pos.x <= DIMX - 1) && (pos.x >= 0) && (pos.y <= DIMX - 1) &&
           (pos.y >= 0);
  }
  std::vector<Actor *> objs;
};