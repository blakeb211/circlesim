#include "spatial_generator.h"
#include "world.h"
bool CellContext::operator==(const CellContext &c) const {
  return this->id == c.id && this->shape == c.shape;
}
bool CellContext::operator!=(const CellContext &c) const {
  return this->id != c.id || this->shape != c.shape;
}

CellContext World::EMPTY_CELL_CONTEXT = CellContext{-1, Shape::QUAD};

/// @brief remove actor segments from each cell it occupies
/// @param a actor pointer
/// @param w world pointer
// Assumes position is valid in the world object already
void World::remove_actor_occupancy(Actor const *const a, World *w) {
  // remove actor from cells it occupied in the world occupation map
  auto center = a->pos;
  auto sz_periph = a->periph.size();

  w->occupation[v2i(static_cast<int>(center.x), static_cast<int>(center.y))] =
      World::EMPTY_CELL_CONTEXT;

  for (int i = 0; i < sz_periph; i++) {
    const auto abs_loc = a->periph[i] + center;
    const auto x = static_cast<int>(abs_loc.x);
    const auto y = static_cast<int>(abs_loc.y);
    w->occupation[v2i(x, y)] = World::EMPTY_CELL_CONTEXT;
  }
}

/// @brief add actor segments to each cell it occupies now
/// @param a actor pointer
/// @param w world pointer
// Assumes position is valid in the world obejct already
void World::add_actor_occupancy(Actor const *const a, World *w) {
  // remove actor from cells it occupied before the move
  // add actor to cells it occupies now
  // add positons to the world occupation map
  auto center = a->pos;
  auto sz_periph = a->periph.size();

  w->occupation[v2i(static_cast<int>(center.x), static_cast<int>(center.y))] =
      CellContext{a->get_id(), a->shape};

  for (int i = 0; i < sz_periph; i++) {
    const auto abs_loc = a->periph[i] + center;
    const auto x = static_cast<int>(abs_loc.x);
    const auto y = static_cast<int>(abs_loc.y);
    w->occupation[v2i(x, y)] = CellContext{a->get_id(), a->shape};
  }
}

size_t World::dimx() const { return DIMX; }

World::~World() {
  for (auto o : objs) {
    o->~Actor();
  }
  std::cout << "World destructor finished\n";
}

World::World(int id, unsigned int dimx) {
  // @TODO move world generation to its own header file
  // @TODO make sure initial placement of object is a valid spot
  Matrix walls = gen_world_bsp(dimx, 222);

  DIMX = dimx;
  auto my_rand = rint_distr(1, 200);
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

        if (walls(x, y) == 1) {
          objs.push_back(
              new Actor(v2{static_cast<float>(x), static_cast<float>(y)}, ang,
                        new UnmovableWallState{}, Shape::QUAD));
          continue;
        }

        if (my_rand(generator) > 197) {
          objs.push_back(
              new Actor(v2{static_cast<float>(x), static_cast<float>(y)}, ang,
                        new WantThreeHatNeighborsState{}, Shape::CIRC));
        }
        if (my_rand(generator) == 1) {
          objs.push_back(
              new Actor(v2{static_cast<float>(x), static_cast<float>(y)}, ang,
                        new RandomMoveState{}, Shape::THREEHAT));
        }
        if (my_rand(generator) == 3) {
          objs.push_back(
              new Actor(v2{static_cast<float>(x), static_cast<float>(y)}, ang,
                        new UnmovableWallState{}, Shape::QUAD));
        }
      } // y loop
    }   // x loop
    break;
  default:
    assert(0);
  };
}

// @brief check that index falls within map coords
bool World::pos_falls_within_map(v2 pos) {
  return (pos.x <= DIMX - 1) && (pos.x >= 0) && (pos.y <= DIMX - 1) &&
         (pos.y >= 0);
}

bool World::pos_valid(Actor *a, v2 pos) {
  // We are on a unit grid.
  // Check if cell is occupied already.
  {
    if (!pos_falls_within_map(pos)) {
      return false;
    }
    v2i pos_int = v2i(std::round(pos.x), std::round(pos.y));
    if (occupation.find(pos_int) == occupation.end()) {
      return true;
    }
    CellContext tmp = occupation[pos_int];
    if (tmp.id == a->get_id() || tmp == World::EMPTY_CELL_CONTEXT) {
      return true;
    }
  }
  return false;
}

std::vector<CellContext> &World::neighbors_of(Actor *a) {
  auto &n = World::neighbors_scratch_vec;
  // get vec of all positions of interest
  std::vector<v2> pos{a->periph};
  for (auto &p : pos) {
    p += a->pos;
  }
  pos.push_back(a->pos); // add center to it

  std::set<v2i> pos_to_evaluate;
  // Collect positions adjacent to all the actors segments
  for (auto p : pos) {
    pos_to_evaluate.insert(v2i(std::round(p.x), std::round(p.y)) + v2i{+1, +0});
    pos_to_evaluate.insert(v2i(std::round(p.x), std::round(p.y)) + v2i{-1, +0});
    pos_to_evaluate.insert(v2i(std::round(p.x), std::round(p.y)) + v2i{+1, +1});
    pos_to_evaluate.insert(v2i(std::round(p.x), std::round(p.y)) + v2i{-1, -1});
    pos_to_evaluate.insert(v2i(std::round(p.x), std::round(p.y)) + v2i{-1, +1});
    pos_to_evaluate.insert(v2i(std::round(p.x), std::round(p.y)) + v2i{+1, -1});
    pos_to_evaluate.insert(v2i(std::round(p.x), std::round(p.y)) + v2i{+0, +1});
    pos_to_evaluate.insert(v2i(std::round(p.x), std::round(p.y)) + v2i{+0, -1});
  }

  // if exist in occupation and unequal to EMPTY_CELL, add to neighbor
  n.clear();
  for (auto pp : pos_to_evaluate) {
    if (occupation.find(pp) != occupation.end()) { // exists in occupation
      CellContext tmp = occupation[pp];
      if (tmp != World::EMPTY_CELL_CONTEXT) { // not empty
        if (tmp.id != a->get_id() &&
            tmp.shape == Shape::THREEHAT) { // not just a piece of current actor
          n.push_back(tmp);
        }
      }
    }
  }

  return n;
}
