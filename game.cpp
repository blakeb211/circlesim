#include "game.h"

void Game::init() {
  // Called once at the start, so create things here
  for (int i = 0; i < 10; i++) {
    bool inverted = i % 2;
    World *w = WorldFactory::create_world_bsp(50ul, (unsigned int)i * 2, 0.05f,
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

