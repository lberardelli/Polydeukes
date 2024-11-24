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
void (*MousePicker::mousePositionCallback)(GLFWwindow*,double,double) = [](GLFWwindow* window, double d, double d2) {};
Ray MousePicker::ray{};
Renderer* MousePicker::renderer{};
Camera* MousePicker::camera{};
Scene* MousePicker::theScene{};
MousePicker::MouseRayCollision MousePicker::collisionData{};
std::shared_ptr<Shape> MousePicker::currentlySelectedShape{};
std::function<void(double,double)> MousePicker::clickCustomization{};
std::function<void(double,double)> MousePicker::rightClickCustomization{};
Camera* LineDrawer::camera;
LineDrawer::LineData LineDrawer::lineData{};
std::vector<std::weak_ptr<Shape>> MeshDragger::passengers{};
glm::vec3 LineDrawer::normal{};

glm::vec3 MeshDragger::computeNewLocation(double mousePosX, double mousePosY) {
    //get the mouse ray
    Ray mouseRay = MousePicker::computeMouseRay(mousePosX, mousePosY);
    //get the plane defined by the camera and the targetShape
    glm::vec3 planeNormal = camera->getDirection();
    glm::vec3 planeSamplePoint = targetShape.lock()->getPosition();
    //ray plane intersection
    return vector::rayPlaneIntersection(Plane(planeNormal, planeSamplePoint), mouseRay);
}

glm::vec3 LineDrawer::computeNewLocation(double mousePosX, double mousePosY, glm::vec3 initialPosition) {
    //get the mouse ray
    Ray mouseRay = MousePicker::computeMouseRay(mousePosX, mousePosY);
    //get the plane defined by the camera and the targetShape
    MousePicker::mousePositionX = mousePosX;
    MousePicker::mousePositionY = mousePosY;
    MousePicker::computeWorldRay();
    std::vector<std::tuple<std::shared_ptr<Shape>, glm::vec3>> candidates = MousePicker::computeRayTriangleCollisions();
    std::sort(candidates.begin(), candidates.end(), [](auto e, auto e2) {
        glm::vec4 res1 = camera->viewingTransformation() * glm::vec4(std::get<1>(e), 1.0f);
        glm::vec4 res2 = camera->viewingTransformation() * glm::vec4(std::get<1>(e2), 1.0f);
        return res2.z < res1.z;
    });
    glm::vec3 planeNormal = glm::normalize(glm::cross(glm::vec3(1.0f,0.0f,0.0f), std::get<1>(candidates[0]) - initialPosition));
    glm::vec3 planeSamplePoint = initialPosition;
    //ray plane intersection
    return vector::rayPlaneIntersection(Plane(planeNormal, planeSamplePoint), mouseRay);
}
