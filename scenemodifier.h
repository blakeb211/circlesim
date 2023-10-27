// ADAPTED FROM: Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef SCENEMODIFIER_H
#define SCENEMODIFIER_H

#include <QtCore/QObject>

#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>

#include <Qt3DExtras/QTorusMesh>
#include <Qt3DExtras/QConeMesh>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>
#include "world.h"

class SceneModifier : public QObject
{

public:
    QMap<size_t, QVector<Qt3DCore::QEntity *>> id2ent;
    void move_object(QVector<Qt3DCore::QEntity *> & v, Actor * o, QVector3D abs_pos);
    void update_pos(World * w);
    explicit SceneModifier(Qt3DCore::QEntity *rootEntity);
    Qt3DCore::QEntity * m_rootEntity;
    ~SceneModifier();

public slots:
    void enableTorus(bool enabled);
    void enableCone(bool enabled);
    void enableCylinder(bool enabled);
    void enableCuboid(bool enabled);
    void enablePlane(bool enabled);
    void enableSphere(bool enabled);

};

#endif // SCENEMODIFIER_H

