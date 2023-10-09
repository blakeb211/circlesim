#pragma once
#include "util.h"
#include "world.h"

class UI;
struct Game;

// Member function handling
#define CALL_MEMBER_FN(object, ptrToMember) ((object).*(ptrToMember))
typedef std::vector<Action *> (UI::*UserInputMemFn)(Game *);
typedef void (UI::*GameInitMemFn)(Game *);
typedef void (UI::*DrawWorldMemFn)(Game *, World *);

struct Game {
  // Decouple game from game engine
  World *active_w;
  size_t active_world_index{0};
  std::vector<World *> worlds;
  bool paused{false};
  bool drawing_neighbors{false};
  bool drawing_grid{false};
  UserInputMemFn process_user_input;
  GameInitMemFn game_init;
  DrawWorldMemFn draw_world;
  bool time_to_cleanup{false}; 
};