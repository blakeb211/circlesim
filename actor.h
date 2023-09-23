#pragma once
#include "util.h"
#include <vector>

using vec_pts = std::vector<olc::vf2d>;
using veci_pts = std::vector<olc::vi2d>;
using v2 = olc::vf2d;
using v2i = olc::vi2d;

class Actor; 
class World;

class State {
  public:
  std::unordered_map<const char *, float> params;
  virtual ~State() {};
  virtual State *handle_input() = 0; // for switching states
  virtual void update(Actor * o, World * w, float dt) = 0;
};

class RandomMoveState : public State {
State * handle_input();
void update(Actor * o, World * w, float dt);
};

enum class Shape {
  CIRC = 0,
  TRI = 1,
  QUAD = 2,
};

class Actor {
  State *state{nullptr};
public:
~Actor() {
  if (state) {
    delete(state);
  }
}
  v2 pos;
  float rot;
  Shape shape;
  Actor() = delete;
  Actor(v2 pos, float rot, State * initial_state, Shape shape = Shape::CIRC);
  void update(World *w, float dt);
};