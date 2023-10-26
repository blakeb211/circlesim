#include "game.h"

void Game::init() {
  // Called once at the start, so create things here
  for (int i = 0; i < 4; i++) {
    bool inverted = 0;
    World *w = WorldFactory::create_world_bsp(40ul, 0, i*30, -i*20, (unsigned int)i * 2, 0.05f,
                                              inverted);
    this->worlds.push_back(w); // so it can be cleaned up
  }
  
  for (int i = 0; i < 4; i++) {
    bool inverted = 1;
    World *w = WorldFactory::create_world_bsp(64ul, 20+i*70, 120, -90, (unsigned int)i * 2, 0.05f,
                                              inverted);
    this->worlds.push_back(w); // so it can be cleaned up
  }

}

/// @brief 
/// @param w 
/// @param dt 
/// @return 
auto Game::update_world(World *w, float dt) -> std::vector<Action *> {
  std::vector<Action *> ret;
  for (auto &o : w->objs) {
    Action *res = o->update(w, dt);
    if (res) {
      ret.push_back(res);
    }
  }
  return ret;
}

