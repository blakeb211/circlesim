#define TWOD 0

#ifdef TWOD
#define OLC_PGE_APPLICATION
#endif

#include <QtCore/QTimer>
#include <QtGui/QGuiApplication>

#include <Qt3DCore/qentity.h>
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

#include "action.h"
#include "actor.h"
#include "game.h"
#include "scenemodifier.h"
#include "world.h"
#include <Qt3DExtras/qfirstpersoncameracontroller.h>
#include <Qt3DExtras/qt3dwindow.h>

void main_loop_3d(SceneModifier *modifier, QWidget * win, Qt3DRender::QCamera * cam, Game *g) {
  // Update your simulation state here
  // ...
  if (g->active_w != g->worlds[g->active_world_index]) {
    g->active_w = g->worlds[g->active_world_index];
    // center cam on current active world
    auto center2d = QVector2D(g->active_w->dimx() / 2, int(g->active_w->dimx() / 2));
    cam->setPosition(QVector3D(center2d.x(), center2d.y(), 60.0f));
    cam->setUpVector(QVector3D(0, 1, 0));
    cam->setViewCenter(QVector3D(center2d.x(), center2d.y(), 0));
    win->setWindowTitle("World: " + QString::number(g->active_world_index));
  }

  Q_ASSERT(g->active_w);
  auto actions = g->update_world(g->active_w, 0.016f);
  Action::run_actions(actions);
  modifier->update_pos(g->active_w);

}

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
#endif

int main(int argc, char **argv) {
  Game *g = new Game();
  g->init();

#if TWOD
  UI demo(g);
  if (demo.Construct(512, 480, 4, 4, false, false))
    demo.Start();
#else

  /* QWidget is the window. Container is the window of the 3d rendering view.
   */
  QApplication app(argc, argv); // event loop
  Qt3DExtras::Qt3DWindow *view = new Qt3DExtras::Qt3DWindow();
  view->defaultFrameGraph()->setClearColor(QColor(QRgb(0x4d4d4f)));
  QWidget *container = QWidget::createWindowContainer(view);
  QSize screenSize = view->screen()->size();
  container->setMinimumSize(QSize(200, 100));
  container->setMaximumSize(screenSize);

  QWidget *widget = new QWidget;
  QHBoxLayout *hLayout = new QHBoxLayout(widget);
  QVBoxLayout *vLayout = new QVBoxLayout();
  vLayout->setAlignment(Qt::AlignTop);
  hLayout->addWidget(container, 1);
  hLayout->addLayout(vLayout);

  widget->setWindowTitle(QStringLiteral("3d viewer"));

  // Root entity
  Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();

  // Camera
  Qt3DRender::QCamera *cameraEntity = view->camera();

  cameraEntity->lens()->setPerspectiveProjection(55.0f, 10.0f / 10.0f, 0.1f,
                                                 1000.0f);
  cameraEntity->setPosition(QVector3D(0, 0, 20.0f));
  cameraEntity->setUpVector(QVector3D(0, 1, 0));
  cameraEntity->setViewCenter(QVector3D(0, 0, 0));

  Qt3DCore::QEntity *lightEntity = new Qt3DCore::QEntity(rootEntity);
  Qt3DRender::QPointLight *light = new Qt3DRender::QPointLight(lightEntity);
  light->setColor("white");
  light->setIntensity(1);
  lightEntity->addComponent(light);
  Qt3DCore::QTransform *lightTransform = new Qt3DCore::QTransform(lightEntity);
  lightTransform->setTranslation(cameraEntity->position());
  lightEntity->addComponent(lightTransform);

  // For camera controls
  Qt3DExtras::QFirstPersonCameraController *camController =
      new Qt3DExtras::QFirstPersonCameraController(rootEntity);
  camController->setCamera(cameraEntity);

  // Scenemodifier
  SceneModifier *modifier = new SceneModifier(rootEntity);

  // Set root object of the scene
  view->setRootEntity(rootEntity);

  // Create control widgets
  QCommandLinkButton *info = new QCommandLinkButton();
  info->setText(QStringLiteral("Clickable label"));
  info->setDescription(QString::fromLatin1("World Information"));
  info->setIconSize(QSize(0, 0));

  QCheckBox *torusCB = new QCheckBox(widget);
  torusCB->setChecked(true);
  torusCB->setText(QStringLiteral("Torus"));

  QCheckBox *coneCB = new QCheckBox(widget);
  coneCB->setChecked(true);
  coneCB->setText(QStringLiteral("Cone"));

  QCheckBox *cylinderCB = new QCheckBox(widget);
  cylinderCB->setChecked(true);
  cylinderCB->setText(QStringLiteral("Cylinder"));

  QCheckBox *cuboidCB = new QCheckBox(widget);
  cuboidCB->setChecked(true);
  cuboidCB->setText(QStringLiteral("Cuboid"));

  QCheckBox *planeCB = new QCheckBox(widget);
  planeCB->setChecked(true);
  planeCB->setText(QStringLiteral("Plane"));

  QCheckBox *sphereCB = new QCheckBox(widget);
  sphereCB->setChecked(true);
  sphereCB->setText(QStringLiteral("Sphere"));

  vLayout->addWidget(info);
  vLayout->addWidget(torusCB);
  vLayout->addWidget(coneCB);
  vLayout->addWidget(cylinderCB);
  vLayout->addWidget(cuboidCB);
  vLayout->addWidget(planeCB);
  vLayout->addWidget(sphereCB);

  /* QObject::connect(torusCB, &QCheckBox::stateChanged, modifier,
                   &SceneModifier::enableTorus);
  QObject::connect(coneCB, &QCheckBox::stateChanged, modifier,
                   &SceneModifier::enableCone);
  QObject::connect(cylinderCB, &QCheckBox::stateChanged, modifier,
                   &SceneModifier::enableCylinder);
  QObject::connect(cuboidCB, &QCheckBox::stateChanged, modifier,
                   &SceneModifier::enableCuboid);
  QObject::connect(planeCB, &QCheckBox::stateChanged, modifier,
                   &SceneModifier::enablePlane);
  QObject::connect(sphereCB, &QCheckBox::stateChanged, modifier,
                   &SceneModifier::enableSphere); */

  torusCB->setChecked(true);
  coneCB->setChecked(true);
  cylinderCB->setChecked(true);
  cuboidCB->setChecked(true);
  planeCB->setChecked(true);
  sphereCB->setChecked(true);

  // Set up a timer for updates
  QTimer timer;
  auto main_loop_functor = std::bind(main_loop_3d, modifier, widget, cameraEntity, g);
  QObject::connect(&timer, &QTimer::timeout, main_loop_functor);
  // Start the timer
  timer.start(16); // Update every 16 milliseconds

  // Show the Qt3D window
  view->show();

  // Show window
  widget->show();
  widget->resize(1200, 800);

  app.exec();

#endif
  return 0;
}
