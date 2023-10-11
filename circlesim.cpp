#define OLC_PGE_APPLICATION
#define TWOD 1

#include <QtGui/QGuiApplication>

/*  #include <Qt3DCore/qentity.h>
 #include <Qt3DRender/qcamera.h>
 #include <Qt3DRender/qcameralens.h>

 #include <QtGui/QScreen>
 #include <QtWidgets/QApplication>
 #include <QtWidgets/QCheckBox>
 #include <QtWidgets/QCommandLinkButton>
 #include <QtWidgets/QHBoxLayout>
 #include <QtWidgets/QWidget>

 #include <Qt3DExtras/qtorusmesh.h>
 #include <Qt3DRender/qeffect.h>
 #include <Qt3DRender/qmaterial.h>
 #include <Qt3DRender/qmesh.h>
 #include <Qt3DRender/qpointlight.h>
 #include <Qt3DRender/qrenderpass.h>
 #include <Qt3DRender/qsceneloader.h>
 #include <Qt3DRender/qtechnique.h>
 #include <Qt3DRender/qtexture.h>

 #include <Qt3DCore/qaspectengine.h>
 #include <Qt3DCore/qtransform.h>

 #include <Qt3DExtras/qforwardrenderer.h>
 #include <Qt3DRender/qrenderaspect.h>

 #include <Qt3DExtras/qfirstpersoncameracontroller.h>
 #include <Qt3DExtras/qt3dwindow.h> */
#include "action.h"
#include "actor.h"
#include "game.h"
#include "world.h"

#if TWOD
// Override base class with your custom functionality
class UI : public olc::PixelGameEngine {
public:
  Game *g;
  UI() = delete;
  UI(Game *g);
  auto get_user_input(Game *g) -> std::vector<Action *>;
  void draw_world_PGE_2d(Game *g, World *w);

public:
  bool OnUserCreate() override;

  bool OnUserUpdate(float fElapsedTime) override;
};

UI::UI(Game *g) : g{g} {}

auto UI::get_user_input(Game *g) -> std::vector<Action *> {
  std::vector<Action *> ret;
  auto hero = g->active_w->objs[0];
  if (GetKey(olc::Key::LEFT).bPressed) {
    ret.push_back(new MoveAction(hero, g->active_w, v2{-1, 0}));
  }
  if (GetKey(olc::Key::RIGHT).bPressed) {
    ret.push_back(new MoveAction(hero, g->active_w, v2{1, 0}));
  }
  if (GetKey(olc::Key::UP).bPressed) {
    ret.push_back(new MoveAction(hero, g->active_w, v2{0, -1}));
  }
  if (GetKey(olc::Key::DOWN).bPressed) {
    ret.push_back(new MoveAction(hero, g->active_w, v2{0, +1}));
  }
  if (GetKey(olc::Key::Q).bHeld) {
    ret.push_back(new QuitAction(g));
  }
  //
  if (GetKey(olc::Key::W).bPressed) {
    g->active_world_index++;
    if (g->active_world_index == g->worlds.size()) {
      g->active_world_index = 0;
    }
  }
  if (GetKey(olc::Key::P).bPressed) {
    g->paused = !g->paused;
  }
  if (GetKey(olc::Key::D).bPressed) {
    g->drawing_neighbors = !g->drawing_neighbors;
  }
  if (GetKey(olc::Key::G).bPressed) {
    g->drawing_grid = !g->drawing_grid;
  }
  return ret;
}

// want a 3d scene where we can move independently to different worlds and
// interact with them
void UI::draw_world_PGE_2d(Game *g, World *w) {
  const auto DIMX = w->dimx();
  const auto xoffset = 50.f;
  const auto xspace = (UI::GetScreenSize().x - 2 * xoffset) / DIMX;
  const auto yoffset = 50.f;
  const auto yspace = (UI::GetScreenSize().y - 2 * yoffset) / DIMX;
  for (const auto &o : w->objs) {
    float x = xoffset + o->pos.x * xspace;
    float y = yoffset + o->pos.y * yspace;
    auto radius = 3;
    v2i facing = v2i{static_cast<int>(radius * cos(o->facing)),
                     static_cast<int>(radius * sin(o->facing))};
    // To make actors with multiple pieces easier to draw
    const auto &perhip = o->periph;
    auto sz = perhip.size();
    State *tmp_circ = new CirclingState(4, 1, 1);
    switch (o->shape) {
    case Shape::CIRC:
      if (typeid(*(o->state)) == typeid(*tmp_circ)) {
        FillCircle(v2i{static_cast<int>(x), static_cast<int>(y)}, radius,
                   o->color);
      } else {
        FillCircle(v2i{static_cast<int>(x), static_cast<int>(y)}, radius,
                   o->color);
      }
      Draw(facing.x + x, facing.y + y, olc::RED);
      break;
    case Shape::QUAD:
      FillRect(v2i{static_cast<int>(x - radius), static_cast<int>(y - radius)},
               v2i{radius * 2, radius * 2}, olc::DARK_YELLOW);
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

    delete tmp_circ;
    // draw text labels
    if (g->drawing_neighbors) {
      if (o->neighbors > -1) {
        DrawString(x, y, std::to_string(o->neighbors));
      }
    }
    if (g->drawing_grid) {
      if (o->shape == Shape::QUAD) {
        continue;
      }
      auto x_string = std::to_string((int)o->pos.x);
      auto y_string = std::to_string((int)o->pos.y);
      auto tot_string = x_string + "," + y_string;
      DrawString(x, y, tot_string, olc::DARK_BLUE);
    }
  }
}

bool UI::OnUserCreate() { return true; }

bool UI::OnUserUpdate(float fElapsedTime) {
  // Called once per frame
  g->active_w = g->worlds[g->active_world_index];
  this->sAppName = "World: " + std::to_string(g->active_world_index);

  if (g->paused) {
    return true;
  }

  Clear(olc::VERY_DARK_GREY);
  std::vector<Action *> user_actions = get_user_input(g);
  Action::run_actions(user_actions);
  std::vector<Action *> ai_actions = g->update_world(g->active_w, fElapsedTime);
  Action::run_actions(ai_actions);

  draw_world_PGE_2d(g, g->active_w);
  if (g->time_to_cleanup) {
    for (auto w : g->worlds) {
      w->~World();
    }
    return false;
  }
  return true;
}
#else
// Three d UI
class UI2 : QMainWindow {};
#endif

int main() {
  Game *g = new Game();
  g->init();

#if TWOD
  UI demo(g);
  if (demo.Construct(512, 480, 4, 4, false, false))
    demo.Start();
#endif
  return 0;
}
