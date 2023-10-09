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

