#include "spatial_generator.h"
#include "util.h"
#include "world.h"
bool CellContext::operator==(const CellContext &c) const {
  return (this->id == c.id) && (this->shape == c.shape);
}
bool CellContext::operator!=(const CellContext &c) const {
  return (this->id != c.id) || (this->shape != c.shape);
}

CellContext World::EMPTY_CELL_CONTEXT = CellContext{-1, Shape::TRI};

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

  w->occupation[v2i(static_cast<int>(std::round(center.x)),
                    static_cast<int>(std::round(center.y)))] =
      CellContext{a->get_id(), a->shape};

  for (int i = 0; i < sz_periph; i++) {
    const auto abs_loc = a->periph[i] + center;
    const auto x = (int)std::round(abs_loc.x);
    const auto y = (int)std::round(abs_loc.y);
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

World::World(size_t dimx) { DIMX = dimx; };

World *WorldFactory::create_world_bsp(size_t dimx, unsigned int seed,
                                      float density, bool inverted) {
  // @TODO move world generation to its own header file
  // @TODO make sure initial placement of object is a valid spot
  Matrix walls = gen_world_bsp(dimx, seed, inverted);
  World *w = new World(dimx);

  auto try_add_actor_at_position = [](Actor *a, World *w, v2i offset) {
    if (w->pos_valid_whole_actor(a, offset)) {
      a->pos += offset;
      World::add_actor_occupancy(a, w);
      w->objs.push_back(a);
    } else {
      delete a;
    }
  };

  float ang = -90 * DEG2RAD;
  // Create hero character first

  auto hero_start_pos = v2{float(dimx / 2), float(dimx - 1)};
  const auto pre_placement_pos = v2{-99, -99};

  Actor *hero = new Actor(pre_placement_pos, ang, nullptr, Shape::QUAD);

  auto offset = hero_start_pos - pre_placement_pos;
  try_add_actor_at_position(hero, w, offset);

  for (int x = 0; x < dimx; x++) {
    for (int y = 0; y < dimx; y++) {
      if (walls(x, y) == 1) {

        Actor *new_obj = new Actor(pre_placement_pos, ang,
                                   new UnmovableWallState{}, Shape::QUAD);
        auto actor_pos = v2{float(x), float(y)};
        try_add_actor_at_position(new_obj, w, actor_pos - pre_placement_pos);

      } else { // not a wall position

        auto density_rand = rint_distr(1, int(1 / density))(generator);
        if (density_rand != 1) {
          continue;
        }

        // do object placement
        auto dice_roll = rint_distr(0, 6)(generator);
        Actor *new_obj =
            dice_roll > 0
                ? new Actor(pre_placement_pos, ang,
                            new WantThreeHatNeighborsState{}, Shape::CIRC)
                : new Actor(pre_placement_pos, ang, new RandomMoveState{},
                            Shape::THREEHAT);

        // make some of the monoactors circlers
        auto dice_roll2 = rint_distr(0, 6)(generator);
        if (new_obj->shape == Shape::CIRC && dice_roll2 == 0) {
          delete new_obj->state;
          new_obj->state = new CirclingState{5, x, y};
        }

        auto actor_pos = v2{float(x), float(y)};
        try_add_actor_at_position(new_obj, w, actor_pos - pre_placement_pos);
      }
    } // y loop
  }   // x loop
  return w;
}

// @brief check that index falls within map coords
bool World::pos_falls_within_map(v2 pos) {
  return (pos.x <= DIMX - 1) && (pos.x >= 0) && (pos.y <= DIMX - 1) &&
         (pos.y >= 0);
}

bool World::single_cell_unoccupied(Actor *a, v2 pos) {
  // We are on a unit grid.
  // Check if cell is occupied already.
  if (!pos_falls_within_map(pos)) {
    return false;
  }
  v2i pos_int = v2i((int)std::round(pos.x), (int)std::round(pos.y));
  // no map entry for location
  if (occupation.find(pos_int) == occupation.end()) {
    return true;
  }
  CellContext tmp = occupation[pos_int];
  // does cell contain part of actor a or the empty cell sentinel
  if (tmp.id == a->get_id() || tmp == World::EMPTY_CELL_CONTEXT) {
    return true;
  }
  return false;
}

bool World::pos_valid_whole_actor(Actor *a, v2 offset) {
  auto new_pos = a->pos + offset;
  auto sz_periph = a->periph.size();
  bool center_valid = this->single_cell_unoccupied(a, new_pos);
  if (!center_valid) {
    return false;
  }
  // If object has any, check if perhipheral segments are valid positions
  // too
  for (int i = 0; i < sz_periph; i++) {
    if (a->visible[i]) {
      if (!this->single_cell_unoccupied(a, a->periph[i] + new_pos))
        return false;
    }
  }
  return true;
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

// Find direction of nearest attractor within given radius
v2 World::direction_nearest(Actor *a, Shape attractor, int radius) {
  // collect offsets
  auto pos = a->pos;
  std::set<v2i> pos_to_check;
  float radius_f = (float)radius;
  for (; radius_f > -1; radius_f--) {
    pos_to_check.insert(pos + v2{radius_f, radius_f});
    pos_to_check.insert(pos + v2{radius_f, 0});
    pos_to_check.insert(pos + v2{0, radius_f});
    pos_to_check.insert(pos + v2{-radius_f, radius_f});
    pos_to_check.insert(pos + v2{-radius_f, 0});
    pos_to_check.insert(pos + v2{radius_f, -radius_f});
    pos_to_check.insert(pos + v2{0, -radius_f});
    pos_to_check.insert(pos + v2{-radius_f, -radius_f});
  }
  // loop through offsets checking Shape and distance
  v2i closest{9999, 9999};
  for (auto _off : pos_to_check) {
    if (this->occupation.find(pos) != this->occupation.end() && this->occupation[pos].shape == attractor) {
      if (_off.norm() < closest.norm()) {
        closest = _off;
      }
    }
  }
  if (closest != v2i{9999, 9999}) {
    auto move_dir = closest - pos;
    move_dir = move_dir / move_dir.norm();
    move_dir.x = std::round(move_dir.x);
    move_dir.y = std::round(move_dir.y);
    // returning an offset
    return move_dir;
  }
  float offsetx = rint_distr(-2, 2)(generator);
  float offsety = rint_distr(-2, 2)(generator);
  return v2(offsetx, offsety);
}
