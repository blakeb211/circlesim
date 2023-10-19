#pragma once
#include "actor.h"
#include <cstdlib>
#include <vector>

struct CellContext {
  int32_t id;
  Shape shape;
  bool operator==(const CellContext &c) const;
  bool operator!=(const CellContext &c) const;
};

namespace std {

template <> struct std::hash<v2i> {
  std::size_t operator()(v2i const &s) const noexcept {
    std::size_t h1 = std::hash<int>{}(s.x);
    std::size_t h2 = std::hash<int>{}(s.y);
    return h1 ^
           (h2 << 1); // or use boost::hash_combine (see Discussion)
                      // https://en.cppreference.com/w/Talk:cpp/utility/hash
  }
};

} // namespace std

class WorldFactory;

class World {
  size_t DIMX;
  std::vector<CellContext> neighbors_scratch_vec;
  World() = delete;
  World(size_t dimx, int x, int y, int z);
  static size_t counter;
public:
  int pos3d[3];
  size_t id;
  friend WorldFactory;
  static CellContext EMPTY_CELL_CONTEXT;
  static void remove_actor_occupancy(Actor const * const a, World * w);
  static void add_actor_occupancy(Actor const * const a, World * w);
  std::vector<Actor *> objs;
  std::string name{};
  // keep updated with every move and rotate action so that neighbor querying is
  // simple
  std::unordered_map<v2i, CellContext> occupation;
  size_t dimx() const;
  ~World();
  bool pos_falls_within_map(v2 pos);
  bool single_cell_unoccupied(Actor *a, v2 pos);
  bool pos_valid_whole_actor(Actor *a, v2 offset);
  std::vector<CellContext>& neighbors_of(Actor *a);
  v2 direction_nearest(Actor * a, Shape attractor, int radius);
};


class WorldFactory {
  /// @brief 
  /// @param dim 
  /// @param seed 
  /// @return 
  public:
  static World * create_world_bsp(size_t dim, int x, int y, int z, unsigned int seed, float density, bool inverted);

};

