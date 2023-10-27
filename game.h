#pragma once
#include "util.h"
#include "world.h"

class UI;
struct Game;

// Game object used by both engines
struct Game {
  World *active_w;
  size_t active_world_index{0};
  std::vector<World *> worlds;
  bool paused{false};
  bool drawing_neighbors{false};
  bool drawing_grid{false};
  bool time_to_cleanup{false}; 
  void init();
  std::vector<Action*> update_world(World *w, float dt);
};

