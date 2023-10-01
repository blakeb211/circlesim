#include "action.h"
#include "actor.h"
#include "world.h"

int32_t Actor::actor_count = 0;

auto UnmovableWallState::handle_input() -> State * { return this; }

auto UnmovableWallState::update(Actor *a, World *w, float dt) -> Action * {return nullptr;};

auto WantThreeHatNeighborsState::handle_input() -> State * { return this; }

auto WantThreeHatNeighborsState::update(Actor *a, World *w, float dt) -> Action * { 
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
    for(int i = 0; i < sz; i++) {
      if (neighbors[i].shape == Shape::THREEHAT) {
        count_three_hat_neighbors++;
      }
    }
      a->neighbors = count_three_hat_neighbors;

      if (a->neighbors < 2) {
        auto randx = static_cast<float>(rint_distr(-2,2)(generator));
        auto randy = static_cast<float>(rint_distr(-2,2)(generator));
        return new MoveAction(a, w, v2{randx,randy});
      }
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
    float offsetx = rint_distr(-2,2)(generator);
    float offsety = rint_distr(-2,2)(generator);
    int case_num = rint_distr(0,1)(generator);
    switch(case_num) {
      case 0:
      return new RotateAction(a, w, PI / 4);
      case 1: 
      return new MoveAction(a, w, v2{offsetx, offsety});
    };
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