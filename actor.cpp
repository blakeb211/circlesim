#include "action.h"
#include "actor.h"
#include "world.h"
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
    float offsetx = rint_distr(-2,2)(generator);
    float offsety = rint_distr(-2,2)(generator);
    int case_num = rint_distr(0,3)(generator);
    switch(case_num) {
      case 0:
      return new RotateAction(a, w, PI / 4);
      case 1: 
      return new MoveAction(a, w, v2{offsetx, 0});
      case 2: 
      return new MoveAction(a, w, v2{0, offsety});
      case 3: 
      return new MoveAction(a, w, v2{offsetx, offsety});
    };
  }
  return nullptr;
}

Actor::Actor(v2 pos, float rot, State *initial_state, Shape shape) {
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