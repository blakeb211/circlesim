#include <vector>
#include <iostream>

class Action {};

struct Game;

class UI;

class World{ };

// Game class definition
typedef std::vector<Action *> (UI::*ExampleMemFnActionRet)(
    Game *);                                                  // Please do this!
typedef void (UI::*GameInitMemFn)(Game *);         // Please do this!
typedef void (UI::*DrawWorldMemFn)(Game *, World *); // Please do this!
#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

class UI {
public:
  ExampleMemFnActionRet fun1;
  GameInitMemFn fun2;

  DrawWorldMemFn fun3;
  // Example constructor
  UI();

  // Default member functions
  std::vector<Action *> defaultUserInputFunc(Game *g);

  void defaultInitGameFunc(Game *g);

  void defaultDrawWorldFunc(Game *g, World *w);
};

struct Game {
  ExampleMemFnActionRet get_user_input;
  GameInitMemFn init_game;
  DrawWorldMemFn draw_world;
};

UI::UI() {}

std::vector<Action *> UI::defaultUserInputFunc(Game *g) { 
  std::cout << __func__ << " called"  << std::endl;

  return {}; }
void UI::defaultInitGameFunc(Game *g) {}

void UI::defaultDrawWorldFunc(Game *g, World *w) {}

int main() {
  UI demo;

  // Now, you can use the assigned member functions
  Game game;
  World * world;

  // set member function pointers
  game.get_user_input = &UI::defaultUserInputFunc;
  game.init_game = &UI::defaultInitGameFunc;
  game.draw_world = &UI::defaultDrawWorldFunc;

  // call mem function pointers 
  CALL_MEMBER_FN(demo, game.get_user_input)(&game);

  /*
  (demo.*demo.init_game_func)(&game);
  std::vector<Action*> actions = (demo.*demo.user_input_func)(&game);
  (demo.*demo.draw_world)(&game, &world);
  */

  return 0;
}
