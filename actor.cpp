#include "actor.h"
#include "world.h"

State *RandomMoveState::handle_input() { return this; }

void RandomMoveState::update(Actor *o, World *w, float dt) {
  // dt in seconds
  float accumulated{0.f};
  if (params.find("timer") != params.end()) {
    accumulated = params["timer"] + dt;
    params["timer"] = accumulated;
  } else {
    // set initially to zero
    params["timer"] = accumulated;
  }

  if (accumulated >= UPDATE_INTERVAL) {
    params["timer"] -= UPDATE_INTERVAL;
    auto new_pos = o->pos + v2{static_cast<float>(rand() % 2), static_cast<float>(rand() % 3)};
    auto new_ang = angle_from_two_pos(o->pos, new_pos);
    // try to move and if it is a bad move try reseting to corner
    if (w->pos_valid(new_pos)) {
      o->pos = new_pos;
      o->rot = new_ang;
    } else {
      auto new_pos2 = v2{0, 0};
      if (w->pos_valid(new_pos2)) {
        o->pos = new_pos2;
        o->rot = new_ang;
      }
    }
  }
}

Actor::Actor(v2 pos, float rot, State * initial_state, Shape shape) {
  this->pos = pos;
  this->rot = rot;
  this->state = initial_state; 
  this->shape = shape;
}

void Actor::update(World *w, float dt) {
  if (state == NULL) {
    // do update
  } else {
    state->update(this, w, dt);
  }
}