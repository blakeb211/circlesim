#include "world.h"

bool CellContext::operator==(const CellContext &c) const {
  return this->id == c.id && this->shape == c.shape;
}
bool CellContext::operator!=(const CellContext &c) const {
  return this->id != c.id || this->shape != c.shape;
}

CellContext World::EMPTY_CELL_CONTEXT = CellContext{-1,Shape::QUAD};

/// @brief remove actor segments from each cell it occupies
/// @param a actor pointer
/// @param w world pointer
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

World::World(int id) : DIMX{14} {
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
                        new WantThreeHatNeighborsState{}, Shape::CIRC));
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
}

// @brief check that index falls within map coords
bool World::pos_falls_within_map(v2 pos) {
  return (pos.x <= DIMX - 1) && (pos.x >= 0) && (pos.y <= DIMX - 1) &&
         (pos.y >= 0);
}

bool World::pos_valid(Actor *a, v2 pos) {
  // @todo Update this to use the occupation map
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
  return pos_falls_within_map(pos);
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
    pos_to_evaluate.insert(v2i(std::round(p.x),std::round(p.y)) + v2i{+1, +0});
    pos_to_evaluate.insert(v2i(std::round(p.x),std::round(p.y)) + v2i{-1, +0});
    pos_to_evaluate.insert(v2i(std::round(p.x),std::round(p.y)) + v2i{+1, +1});
    pos_to_evaluate.insert(v2i(std::round(p.x),std::round(p.y)) + v2i{-1, -1});
    pos_to_evaluate.insert(v2i(std::round(p.x),std::round(p.y)) + v2i{-1, +1});
    pos_to_evaluate.insert(v2i(std::round(p.x),std::round(p.y)) + v2i{+1, -1});
    pos_to_evaluate.insert(v2i(std::round(p.x),std::round(p.y)) + v2i{+0, +1});
    pos_to_evaluate.insert(v2i(std::round(p.x),std::round(p.y)) + v2i{+0, -1});
  }

  // if exist in occupation and unequal to EMPTY_CELL, add to neighbor
  n.clear();
  for (auto pp : pos_to_evaluate) {
    if (occupation.find(pp) != occupation.end()) { // exists in occupation
      CellContext tmp = occupation[pp];
      if (tmp != World::EMPTY_CELL_CONTEXT) { // not empty
        if (tmp.id != a->get_id() && tmp.shape==Shape::THREEHAT) { // not just a piece of current actor
          n.push_back(tmp);
        }
      }
    }
  }

  return n;
}