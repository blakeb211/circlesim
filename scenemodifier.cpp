// ADAPTED FROM: Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "scenemodifier.h"
#include <Qt3DRender/QGeometryRenderer>
#include <QtCore/QDebug>
#include <QtCore/QTime>

Qt3DCore::QEntity *create_and_add_torus(Qt3DCore::QEntity *root) {
  // Torus shape data
  //! [0]
  auto m_torus = new Qt3DExtras::QTorusMesh();
  m_torus->setRadius(0.75f);
  m_torus->setMinorRadius(0.4f);
  m_torus->setRings(20);
  m_torus->setSlices(10);
  //! [0]

  // TorusMesh Transform
  //! [1]
  Qt3DCore::QTransform *torusTransform = new Qt3DCore::QTransform();
  torusTransform->setScale(0.5f);
  torusTransform->setRotation(
      QQuaternion::fromAxisAndAngle(QVector3D(0.0f, 1.0f, 0.0f), 25.0f));
  torusTransform->setTranslation(QVector3D(5.0f, 4.0f, 0.0f));
  //! [1]

  //! [2]
  Qt3DExtras::QPhongMaterial *torusMaterial = new Qt3DExtras::QPhongMaterial();
  torusMaterial->setDiffuse(QColor(QRgb(0x1E90FF)));
  //! [2]
  {
    // Torus
    //! [3]
    auto torusEntity = new Qt3DCore::QEntity(root);
    torusEntity->addComponent(m_torus);
    torusEntity->addComponent(torusMaterial);
    torusEntity->addComponent(torusTransform);
    //! [3]
    return torusEntity;
  }
}

Qt3DCore::QEntity *create_and_add_cone(Qt3DCore::QEntity *root) {
  // Cone shape data
  Qt3DExtras::QConeMesh *cone = new Qt3DExtras::QConeMesh();
  cone->setTopRadius(0.5);
  cone->setBottomRadius(1);
  cone->setLength(3);
  cone->setRings(50);
  cone->setSlices(20);

  // ConeMesh Transform
  Qt3DCore::QTransform *coneTransform = new Qt3DCore::QTransform();
  coneTransform->setScale(0.5f);
  coneTransform->setRotation(
      QQuaternion::fromAxisAndAngle(QVector3D(1.0f, 0.0f, 0.0f), 45.0f));
  coneTransform->setTranslation(QVector3D(0.0f, 4.0f, -1.5));

  Qt3DExtras::QPhongMaterial *coneMaterial = new Qt3DExtras::QPhongMaterial();
  coneMaterial->setDiffuse(QColor(QRgb(0x928327)));

  // Cone
  {
    auto coneEntity = new Qt3DCore::QEntity(root);
    coneEntity->addComponent(cone);
    coneEntity->addComponent(coneMaterial);
    coneEntity->addComponent(coneTransform);
    return coneEntity;
  }
}

Qt3DCore::QEntity *create_and_add_cylinder(Qt3DCore::QEntity *root) {
  // Cylinder shape data
  Qt3DExtras::QCylinderMesh *cylinder = new Qt3DExtras::QCylinderMesh();
  cylinder->setRadius(1);
  cylinder->setLength(3);
  cylinder->setRings(100);
  cylinder->setSlices(20);

  // CylinderMesh Transform
  Qt3DCore::QTransform *cylinderTransform = new Qt3DCore::QTransform();
  cylinderTransform->setScale(0.5f);
  cylinderTransform->setRotation(
      QQuaternion::fromAxisAndAngle(QVector3D(1.0f, 0.0f, 0.0f), 45.0f));
  cylinderTransform->setTranslation(QVector3D(-5.0f, 4.0f, -1.5));

  Qt3DExtras::QPhongMaterial *cylinderMaterial =
      new Qt3DExtras::QPhongMaterial();
  cylinderMaterial->setDiffuse(QColor(QRgb(0x928327)));

  // Cylinder
  {
    auto cylinderEntity = new Qt3DCore::QEntity(root);
    cylinderEntity->addComponent(cylinder);
    cylinderEntity->addComponent(cylinderMaterial);
    cylinderEntity->addComponent(cylinderTransform);
    return cylinderEntity;
  }
}

Qt3DCore::QEntity *create_and_add_cuboid(Qt3DCore::QEntity *root) {
  // Cuboid shape data
  Qt3DExtras::QCuboidMesh *cuboid = new Qt3DExtras::QCuboidMesh();

  // CuboidMesh Transform
  Qt3DCore::QTransform *cuboidTransform = new Qt3DCore::QTransform();
  cuboidTransform->setScale(0.5f);
  cuboidTransform->setTranslation(QVector3D(5.0f, -4.0f, 0.0f));

  Qt3DExtras::QPhongMaterial *cuboidMaterial = new Qt3DExtras::QPhongMaterial();
  cuboidMaterial->setDiffuse(QColor(QRgb(0x662423)));

  // Cuboid
  {
    auto cuboidEntity = new Qt3DCore::QEntity(root);
    cuboidEntity->addComponent(cuboid);
    cuboidEntity->addComponent(cuboidMaterial);
    cuboidEntity->addComponent(cuboidTransform);
    return cuboidEntity;
  }
}

Qt3DCore::QEntity *create_and_add_plane(Qt3DCore::QEntity *root) {
  // Plane shape data
  Qt3DExtras::QPlaneMesh *planeMesh = new Qt3DExtras::QPlaneMesh();
  planeMesh->setWidth(2);
  planeMesh->setHeight(2);

  // Plane mesh transform
  Qt3DCore::QTransform *planeTransform = new Qt3DCore::QTransform();
  planeTransform->setScale(2.f);
  planeTransform->setRotation(
      QQuaternion::fromAxisAndAngle(QVector3D(1.0f, 0.0f, 0.0f), 45.0f));
  planeTransform->setTranslation(QVector3D(0.0f, -4.0f, 0.0f));

  Qt3DExtras::QPhongMaterial *planeMaterial = new Qt3DExtras::QPhongMaterial();
  planeMaterial->setDiffuse(QColor(QRgb(0xa69929)));

  // Plane
  {
    auto m_planeEntity = new Qt3DCore::QEntity(root);
    m_planeEntity->addComponent(planeMesh);
    m_planeEntity->addComponent(planeMaterial);
    m_planeEntity->addComponent(planeTransform);
    return m_planeEntity;
  }
}

Qt3DCore::QEntity *create_and_add_sphere(Qt3DCore::QEntity *root) {
  // Sphere shape data
  Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh();
  sphereMesh->setRings(10);
  sphereMesh->setSlices(10);
  sphereMesh->setRadius(2);

  // Sphere mesh transform
  Qt3DCore::QTransform *sphereTransform = new Qt3DCore::QTransform();

  sphereTransform->setScale(0.25f);
  sphereTransform->setTranslation(QVector3D(-5.0f, -4.0f, 0.0f));

  Qt3DExtras::QPhongMaterial *sphereMaterial = new Qt3DExtras::QPhongMaterial();
  sphereMaterial->setDiffuse(QColor(QRgb(0xF69329)));

  // Sphere
  {
    auto sphereEntity = new Qt3DCore::QEntity(root);
    sphereEntity->addComponent(sphereMesh);
    sphereEntity->addComponent(sphereMaterial);
    sphereEntity->addComponent(sphereTransform);
    return sphereEntity;
  }
}

// Modifer adds renderable entities to the 3DWindow (view)'s root entity
SceneModifier::SceneModifier(Qt3DCore::QEntity *rootEntity)
    : m_rootEntity(rootEntity) {}
void SceneModifier::move_object(QVector<Qt3DCore::QEntity *> &v, Actor *o,
                                QVector3D abs_pos) {
  // Move a qentity (used only for 3d drawing) to a specific position
  // @TODO: Update this function to move via the list object including the
  // periph parts of the threehats
  auto num_parts = v.size();
  for (size_t i = 0; i < num_parts; i++) {
    Qt3DCore::QTransform *transform = nullptr;
    const auto &components = v[i]->components();
    for (Qt3DCore::QComponent *component : components) {
      transform = qobject_cast<Qt3DCore::QTransform *>(component);
      if (transform) {
        // Successfully found a QTransform component
        break;
      }
    }
    // Now you can use the transform if it is not nullptr
    if (transform) {
      if (i == 0) {
        transform->setTranslation(abs_pos);
      } else {
        // set periph pos
        transform->setTranslation(
            abs_pos + QVector3D{o->periph[i - 1].x, o->periph[i - 1].y, 0});
      }
    } else {
      qDebug() << "No transform found on entity " << v[i]->id();
    }
  }
}

void SceneModifier::update_pos(World *w) {
  // hold a single world's 3d objects for drawing with the 3d frontend
  for (auto &o : w->objs) {
    auto id = o->get_id();
    auto &pos_2d = o->pos; // flip y to match the 2d engine
    QVector3D pos{pos_2d.x + float(w->pos3d[0]),
                  (w->dimx() - 1) - pos_2d.y + float(w->pos3d[1]),
                  float(w->pos3d[2])};
    if (!id2ent.contains(id)) {
      // new up an entity for it
      switch (o->shape) {
      case Shape::CIRC:
        id2ent[id] = QVector<Qt3DCore::QEntity *>{};
        id2ent[id].push_back(create_and_add_torus(m_rootEntity));
        break;
      case Shape::QUAD:
        id2ent[id] = QVector<Qt3DCore::QEntity *>{};
        id2ent[id].push_back(create_and_add_cuboid(m_rootEntity));
        break;
      case Shape::THREEHAT:
        id2ent[id] = QVector<Qt3DCore::QEntity *>{};
        // add center piece
        id2ent[id].push_back(create_and_add_sphere(m_rootEntity));
        // add peripherals
        for (auto &oo : o->periph) {
          UNUSED(oo);
          id2ent[id].push_back(create_and_add_sphere(m_rootEntity));
        }
        break;
      case Shape::TRI:
        id2ent[id] = QVector<Qt3DCore::QEntity *>{};
        id2ent[id].push_back(create_and_add_cone(m_rootEntity));
        break;
      };
      // qDebug() << "creating an entity at " << QTime::currentTime();
    }
    // set 3d position of object
    move_object(id2ent[id], o, pos);
  }
}

SceneModifier::~SceneModifier() {}

//! [4]
void SceneModifier::enableTorus(bool enabled) {}
//! [4]

void SceneModifier::enableCone(bool enabled) {}

void SceneModifier::enableCylinder(bool enabled) {}

void SceneModifier::enableCuboid(bool enabled) {}

void SceneModifier::enablePlane(bool enabled) {}

void SceneModifier::enableSphere(bool enabled) {}
