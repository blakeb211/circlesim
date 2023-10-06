#pragma once
#include "util.h"
#include <vector>

using vec_pts = std::vector<olc::vf2d>;
using veci_pts = std::vector<olc::vi2d>;
using v2 = olc::vf2d;
using v2i = olc::vi2d;

class Actor;
class World;
class Action;

class State {
public:
  std::unordered_map<const char *, float> params;
  virtual ~State(){};
  virtual State *handle_input() = 0; // for switching states
  virtual Action *update(Actor *o, World *w, float dt) = 0;
};

class RandomMoveState : public State {
  State *handle_input();
  Action *update(Actor *o, World *w, float dt);
};

class WantThreeHatNeighborsState : public State {
  State *handle_input();
  Action *update(Actor *o, World *w, float dt);
};

class UnmovableWallState : public State {
  State *handle_input();
  Action *update(Actor *o, World *w, float dt);
};

class CirclingState : public State {
  unsigned int radius;
  v2i center;
  State *handle_input();
  Action *update(Actor *o, World *w, float dt);
  public:
  CirclingState() = delete;
  CirclingState(unsigned int radius, int centerx, int centery) : radius{radius}, center{v2i{centerx,centery}}{}
};

enum class Shape {
  CIRC = 0,
  TRI = 1,
  QUAD = 2,
  THREEHAT = 3,
};

class Actor {
  static int32_t actor_count;

public:
  olc::Pixel color{olc::WHITE};
  int32_t id{-1};
  State *state{nullptr};
  int neighbors{-1};
  ~Actor() {
    if (state) {
      delete (state);
    }
  }
  int32_t get_id() const { return id; }
  v2 pos; // center of item in absolute unit dimensions
  // invariant: periph.size() == visible.size()
  std::vector<v2> periph;    // non center segments in coords relative to center
  std::vector<bool> visible; // visibility of non-central elements
  float facing;
  Shape shape;
  Actor() = delete;
  Actor(v2 pos, float rot, State *initial_state, Shape shape = Shape::CIRC);
  Action *update(World *w, float dt);
};
