#define OLC_PGE_APPLICATION
#include "action.h"
#include "actor.h"
#include "util.h"
#include "world.h"

// Override base class with your custom functionality
class Example : public olc::PixelGameEngine {
  World *active_w;
  std::vector<World *> worlds;
  bool paused{false};
  bool drawing_neighbors{false};

  auto get_user_input() -> std::vector<Action *> {
    std::vector<Action *> ret;
    auto hero = active_w->objs[0];
    if (GetKey(olc::Key::LEFT).bPressed) {
      ret.push_back(new MoveAction(hero, active_w, v2{-1, 0}));
    }
    if (GetKey(olc::Key::RIGHT).bPressed) {
      ret.push_back(new MoveAction(hero, active_w, v2{1, 0}));
    }
    if (GetKey(olc::Key::UP).bPressed) {
      ret.push_back(new MoveAction(hero, active_w, v2{0, -1}));
    }
    if (GetKey(olc::Key::DOWN).bPressed) {
      ret.push_back(new MoveAction(hero, active_w, v2{0, +1}));
    }
    if (GetKey(olc::Key::Q).bHeld) {
      ret.push_back(new QuitAction());
    }
    return ret;
  }

  // want a 3d scene where we can move independently to different worlds and
  // interact with them
  void draw_world_2d(World *w) {
    const auto DIMX = w->dimx();
    const auto xoffset = 50.f;
    const auto xspace = (Example::GetScreenSize().x - 2 * xoffset) / DIMX;
    const auto yoffset = 50.f;
    const auto yspace = (Example::GetScreenSize().y - 2 * yoffset) / DIMX;
    for (const auto &o : w->objs) {
      float x = xoffset + o->pos.x * xspace;
      float y = yoffset + o->pos.y * yspace;
      auto radius = 3;
      v2i facing = v2i{static_cast<int>(radius * cos(o->facing)),
                       static_cast<int>(radius * sin(o->facing))};
      // To make actors with multiple pieces easier to draw
      const auto &perhip = o->periph;
      auto sz = perhip.size();
      switch (o->shape) {
      case Shape::CIRC:
        FillCircle(v2i{static_cast<int>(x), static_cast<int>(y)}, radius,
                   olc::BLUE);
        Draw(facing.x + x, facing.y + y, olc::RED);
        break;
      case Shape::QUAD:
        DrawRect(
            v2i{static_cast<int>(x - radius), static_cast<int>(y - radius)},
            v2i{radius * 2, radius * 2}, olc::MAGENTA);
        break;
      case Shape::THREEHAT:
        FillCircle(v2i{static_cast<int>(x), static_cast<int>(y)}, radius,
                   olc::DARK_GREEN);
        Draw(v2i{static_cast<int>(x), static_cast<int>(y)});
        for (int i = 0; i < sz; i++) {
          if (o->visible[i])
            FillCircle(v2i{static_cast<int>(x + perhip[i].x * xspace),
                           static_cast<int>(y + perhip[i].y * yspace)},
                       radius, olc::DARK_GREEN);
        }
        break;
      case Shape::TRI:
        Draw(v2i{static_cast<int>(x), static_cast<int>(y)});
        break;
      };
      // draw text labels
      if (drawing_neighbors) {
        if (o->neighbors > -1) {
          DrawString(x, y, std::to_string(o->neighbors));
        }
      }
    }
  }

  auto update_world(World *w, float dt) -> std::vector<Action *> {
    std::vector<Action *> ret;
    for (auto &o : w->objs) {
      Action *res = o->update(w, dt);
      if (res) {
        ret.push_back(res);
      }
    }
    return ret;
  }

public:
  Example() {
    // Name your application
    sAppName = "2d version";
  }

public:
  bool OnUserCreate() override {
    // Called once at the start, so create things here
    active_w = new World(42, 100u);
    worlds.push_back(active_w); // so it can be cleaned up
    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override {
    if (GetKey(olc::Key::P).bPressed) {
      paused = !paused;
    }
    if (GetKey(olc::Key::D).bPressed) {
      drawing_neighbors = !drawing_neighbors;
    }
    if (paused) {
      return true;
    }
    Clear(olc::VERY_DARK_RED);
    // Called once per frame
    std::vector<Action *> user_actions = get_user_input();
    for (auto &a : user_actions) {
      a->execute();
      delete (a);
    }
    std::vector<Action *> ai_actions = update_world(active_w, fElapsedTime);
    for (auto &a : ai_actions) {
      a->execute();
      delete (a);
    }

    draw_world_2d(active_w);
    if (g_time_to_cleanup) {
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
  if (demo.Construct(512, 480, 4, 4, false, false))
    demo.Start();
  return 0;
}