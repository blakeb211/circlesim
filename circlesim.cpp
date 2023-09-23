#define OLC_PGE_APPLICATION
#include "actor.h"
#include "util.h"
#include "world.h"
#include "action.h"

// Override base class with your custom functionality
class Example : public olc::PixelGameEngine {
  World *active_w;
  std::vector<World*> worlds;

auto get_user_input() -> std::vector<Action*> {
  std::vector<Action*> ret;
  auto hero = active_w->objs[0];
  if (GetKey(olc::Key::LEFT).bPressed) {
    ret.push_back(new MoveAction(hero, active_w, v2{-1,0}));
  } 
  if (GetKey(olc::Key::RIGHT).bPressed) {
    ret.push_back(new MoveAction(hero, active_w, v2{1,0}));
  } 
  if (GetKey(olc::Key::UP).bPressed) {
    ret.push_back(new MoveAction(hero, active_w, v2{0,-1}));
  } 
  if (GetKey(olc::Key::DOWN).bPressed) {
    ret.push_back(new MoveAction(hero, active_w, v2{0,+1}));
  } 
  if (GetKey(olc::Key::Q).bHeld) {
    ret.push_back(new QuitAction());
  } 
  return ret;
}

// want a 3d scene where we can move independently to different worlds and interact with them
void draw_world_2d(World * w) {
    const auto DIMX = w->dimx();
    const auto xoffset = 50.f;
    const auto xspace = (Example::GetScreenSize().x - 2*xoffset) / DIMX;
    const auto yoffset = 50.f;
    const auto yspace = (Example::GetScreenSize().y - 2*yoffset) / DIMX;
    for (const auto &o : w->objs) {
      float x = xoffset + o->pos.x * xspace;
      float y = yoffset + o->pos.y * yspace;
      auto radius = 3;
      v2i facing = v2i{static_cast<int>(radius*cos(o->rot)), static_cast<int>(radius*sin(o->rot))};
        switch(o->shape) {
            case Shape::CIRC:
            DrawCircle(v2i{static_cast<int>(x), static_cast<int>(y)}, radius, olc::BLUE);
            Draw(facing.x + x, facing.y + y, olc::RED);
            break;
            case Shape::QUAD:
            DrawRect(v2i{static_cast<int>(x-3), static_cast<int>(y-3)}, v2i{6,6}, olc::DARK_MAGENTA);
            break;
            case Shape::TRI:
            Draw(v2i{static_cast<int>(x), static_cast<int>(y)});
            break;
        }
    }
}

void update_world(World * w, float dt) {
    for (auto & o : w->objs) {
        o->update(w, dt);
    }
}

public:
  Example() {
    // Name your application
    sAppName = "2d version";
  }

public:
  bool OnUserCreate() override {
    // Called once at the start, so create things here
    active_w = new World(42);
    worlds.push_back(active_w); // so it can be cleaned up
    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override {
    // Called once per frame
    std::vector<Action*> actions = get_user_input();
    for (auto & a : actions) {
      a->execute();
      delete(a);
    }
    update_world(active_w, fElapsedTime);
    Clear(olc::DARK_YELLOW);
    draw_world_2d(active_w);
    
    if (time_to_cleanup) {
      for (auto w : worlds) {
        w->~World();
      }
      return false;
    }
    return true;
  }
};

int main() {
  Example demo;
  if (demo.Construct(512, 480, 4, 4,false,true))
    demo.Start();
  return 0;
}