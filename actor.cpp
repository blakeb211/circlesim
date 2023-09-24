#include "action.h"
#include "actor.h"
#include "world.h"
State *RandomMoveState::handle_input() { return this; }

Action *RandomMoveState::update(Actor *o, World *w, float dt) {
  // dt in seconds
  if (params.find("timer") != params.end()) {
    params["timer"] = params["timer"] + dt;
  } else {
    // set initially to zero
    params["timer"] = 0.f;
  }

  if (params["timer"] >= UPDATE_INTERVAL) {
    params["timer"] -= UPDATE_INTERVAL;
    v2 offset{static_cast<float>(rand() % 2 - 1), static_cast<float>(rand() % 3 - 2)};
    auto new_pos = o->pos + offset;
    auto new_ang = angle_from_two_pos(o->pos, new_pos);
    // Try to move and if it is a bad move try moving to 0,0.
    // Else, do not move.
    if (w->pos_valid(new_pos)) {
      return new MoveAction(o, w, offset, new_ang);
    } else {
      auto new_pos2 = v2{0, 0};
      if (w->pos_valid(new_pos2)) {
        return new MoveAction(o, w, new_pos2 - o->pos, new_ang);
      }
    }
  }
  return nullptr;
}

Actor::Actor(v2 pos, float rot, State *initial_state, Shape shape) {
  this->pos = pos;
  this->rot = rot;
  this->state = initial_state;
  this->shape = shape;
  switch (shape) {
  case Shape::THREEHAT:
    periph = std::vector<v2>({{-1, 0}, {+1, 0}, {0, -1}});
    visible = std::vector<bool>({true, true, true});
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