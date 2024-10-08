//
//  mouse.cpp
//  LearnOpenGLProject1
//
//  Created by Lawrence Berardelli on 2024-02-16.
//

#include <stdio.h>

#include "MousePicker.h"

int MousePicker::mousePositionX{};
int MousePicker::mousePositionY{};
Ray MousePicker::ray{};
Renderer* MousePicker::renderer{};
Camera* MousePicker::camera{};
Scene* MousePicker::theScene{};
std::shared_ptr<Shape> MousePicker::targetShape{};
std::function<void(double,double)> MousePicker::clickCustomization{};


glm::vec3 MeshDragger::computeNewLocation(double mousePosX, double mousePosY) {
    //get the mouse ray
    Ray mouseRay = MousePicker::computeMouseRay(mousePosX, mousePosY);
    //get the plane defined by the camera and the targetShape
    glm::vec3 planeNormal = camera->getDirection();
    glm::vec3 planeSamplePoint = targetShape->getPosition();
    //ray plane intersection
    return vector::rayPlaneIntersection(Plane(planeNormal, planeSamplePoint), mouseRay);
}