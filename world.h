#pragma once
#include "actor.h"
#include <cstdlib>
#include <vector>

struct cell_context {
  int32_t id;
  Shape shape;
};

namespace std {

template<> struct std::hash<v2i> {
    std::size_t operator()(v2i const& s) const noexcept {
        std::size_t h1 = std::hash<int>{}(s.x);
        std::size_t h2 = std::hash<int>{}(s.y);
        return h1 ^ (h2 << 1); // or use boost::hash_combine (see Discussion) https://en.cppreference.com/w/Talk:cpp/utility/hash
    }
};

}


class World {
  size_t DIMX;
public:
  std::vector<Actor *> objs;
  std::string name{};
  // keep updated with every move and rotate action so that neighbor querying is simple
  std::unordered_map<v2i, cell_context> occupation; 
  size_t dimx() const { return DIMX; }
  ~World() {
    for (auto o : objs) {
      o->~Actor();
    }
    std::cout << "World destructor finished\n";
  }
  World() = delete;
  World(int id) : DIMX{300} {
    // @TODO set unorderded map capacity
    auto my_rand = rint_distr(1, 100);
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
          } else if (my_rand(generator) == 1) {
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
  std::vector<size_t> neighbors_of(Actor * a) { 
    return std::vector<size_t>{{1,2}};
  }
  };

