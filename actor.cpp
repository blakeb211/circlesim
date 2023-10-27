#include "action.h"
#include "actor.h"
#include "world.h"

// Actors act according to what state they are in. If they have no state then
// their own update method is called.
// There is a little map on each state so that variables can be created
// ad hoc as entries in the map.

int32_t Actor::actor_count = 0;

auto UnmovableWallState::handle_input() -> State * { return this; }

auto UnmovableWallState::update(Actor *a, World *w, float dt) -> Action * {
  return nullptr;
};

auto WantThreeHatNeighborsState::handle_input() -> State * { return this; }

auto WantThreeHatNeighborsState::update(Actor *a, World *w, float dt)
    -> Action * {
  // dt in seconds
  if (params.find("timer") != params.end()) {
    params["timer"] = params["timer"] + dt;
  } else {
    // set initially to zero
    params["timer"] = 0.f;
  }

  if (params["timer"] >= UPDATE_INTERVAL) {
    params["timer"] -= UPDATE_INTERVAL;

    auto neighbors = w->neighbors_of(a);
    auto sz = neighbors.size();
    int count_three_hat_neighbors{0};
    for (int i = 0; i < sz; i++) {
      if (neighbors[i].shape == Shape::THREEHAT) {
        count_three_hat_neighbors++;
      }
    }
    a->neighbors = count_three_hat_neighbors;

    if (a->neighbors < 1) {
      // move in direction of a threehat if within certain radius
      v2 move_offset = w->direction_nearest(a, Shape::THREEHAT, 8);
      a->color = olc::DARK_BLUE;
      return new MoveAction(a, w, move_offset);
    }
    a->color = olc::BLUE;
    return nullptr;
  }
  return nullptr;
}

auto RandomMoveState::handle_input() -> State * { return this; }

auto RandomMoveState::update(Actor *a, World *w, float dt) -> Action * {
  // dt in seconds
  if (params.find("timer") != params.end()) {
    params["timer"] = params["timer"] + dt;
  } else {
    // set initially to zero
    params["timer"] = 0.f;
  }

  if (params["timer"] >= UPDATE_INTERVAL) {
    params["timer"] -= UPDATE_INTERVAL;
    v2 offset{static_cast<float>(rand() % 3 - 1),
              static_cast<float>(rand() % 3 - 1)};
    float offsetx = rint_distr(-2, 2)(generator);
    float offsety = rint_distr(-2, 2)(generator);
    int case_num = rint_distr(0, 1)(generator);
    switch (case_num) {
    case 0:
      return new RotateAction(a, w, PI / 4);
    case 1:
      return new MoveAction(a, w, v2{offsetx, offsety});
    };
  }
  return nullptr;
}

auto CirclingState::handle_input() -> State * { return this; }

Action *CirclingState::update(Actor *o, World *w, float dt) {
  // dt in seconds
  if (params.find("timer") != params.end()) {
    params["timer"] = params["timer"] + dt;
  } else {
    // set initially to zero
    params["timer"] = 0.f;
  }

  if (params["timer"] >= UPDATE_INTERVAL) {
    params["timer"] -= UPDATE_INTERVAL;

    if (params.find("theta") == params.end()) {
      params["theta"] = 0.f;
    }
    if (params.find("direction") == params.end()) {
      params["direction"] = 1;
    }

    params["theta"] += params["direction"] * PI / 8;

    auto new_x = center.x + cos(params["theta"]) * radius;
    auto new_y = center.y + sin(params["theta"]) * radius;
    auto offset = v2{std::round(float(new_x - o->pos.x)),
                     std::round(float(new_y - o->pos.y))};
    auto new_pos = v2{(float)new_x, (float)new_y};
    if (w->single_cell_unoccupied(o, new_pos)) {
      o->color = olc::WHITE;
      return new MoveAction(o, w, offset);
    } else {
      // reset theta and switch direction if position not valid
      o->color = olc::GREY;
      params["direction"] *= -1;
    }
  }
  return nullptr;
}

Actor::Actor(v2 pos, float rot, State *initial_state, Shape shape) {
  this->id = actor_count++;
  this->pos = pos;
  this->facing = rot;
  this->state = initial_state;
  this->shape = shape;
  switch (shape) {
  case Shape::THREEHAT:
    periph = std::vector<v2>({{-1, 0}, {+1, 0}, {0, -1}});
    visible = std::vector<bool>({true, true, true});
    assert(periph.size() == visible.size());
    break;
  default:
    break;
  };
}

auto Actor::update(World *w, float dt) -> Action * {
  if (state == NULL) {
    // do update
    return nullptr;
  } else {
    return state->update(this, w, dt);
  }
  assert(0); // should be unreachable
  return nullptr;
}
