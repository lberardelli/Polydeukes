//
//  mouse.h
//  LearnOpenGLProject1
//
//  Created by Lawrence Berardelli on 2024-02-16.
//

#ifndef mouse_h
#define mouse_h

#include <glm.hpp>
#include "../view/renderer.h"
#include "../view/ScreenHeight.h"
#include "../model/Camera.h"
#include "../model/Scene.h"
#include "../model/shape.h"
#include "Arcball.h"
#include "../model/vector.h"

#include <vector>
#include <algorithm>
#include <tuple>
#include <functional>

/*
 the top level mouse control module is the picker, from there we can decide which control we want
 e.g. arcball, object mover, etc in future
*/

class MeshDragger {
private:
    static std::weak_ptr<Shape> targetShape;
    static std::vector<std::weak_ptr<Shape>> passengers;
    
public:
    
    static Camera* camera;
    
    static void registerMousePositionCallback(GLFWwindow* window, std::weak_ptr<Shape> shape) {
        glfwSetCursorPosCallback(window, meshDraggerPositionCallback);
        targetShape = shape;
    }
    
    static void registerMousePositionCallback(GLFWwindow* window, std::weak_ptr<Shape> shape, std::vector<std::weak_ptr<Shape>> passengers) {
        glfwSetCursorPosCallback(window, meshDraggerPositionCallback);
        targetShape = shape;
        MeshDragger::passengers = passengers;
    }
    
    static void meshDraggerPositionCallback(GLFWwindow* window, double mousePosX, double mousePosY) {
        glm::vec3 newPosition = computeNewLocation(mousePosX, mousePosY);
        glm::vec3 delta = newPosition - targetShape.lock()->getPosition();
        targetShape.lock()->updateModellingTransform(glm::translate(glm::mat4(1.0f), delta));
        targetShape.lock()->onDrag();
        for (auto passenger : passengers) {
            passenger.lock()->updateModellingTransform(glm::translate(glm::mat4(1.0f), delta));
        }
    }
    
    static glm::vec3 computeNewLocation(double mousePosX, double mousePosY);
    
    MeshDragger(Camera* camera) {
        MeshDragger::camera = camera;
    }
};

class LineDrawer {
    
public:
    struct LineData {
        std::weak_ptr<Shape> geometry;
        glm::vec3 startPosition;
        glm::vec3 endPosition;
    };
    
    static Camera* camera;
    static LineData lineData;
    static glm::vec3 normal;
    
    static glm::vec3 computeNewLocation(double mousePosX, double mousePosY, glm::vec3 initialPosition);
    
    static void registerMousePositionCallback(GLFWwindow* window, glm::vec3 startPosition, std::weak_ptr<Shape> geometry) {
        glfwSetCursorPosCallback(window, lineDrawerPositionCallback);
        normal = camera->getDirection();
        lineData.startPosition = startPosition;
        lineData.geometry = geometry;
    }
    
    static void registerMousePositionCallback(GLFWwindow* window, std::weak_ptr<Shape> shape, std::weak_ptr<Shape> geometry) {
        registerMousePositionCallback(window, shape.lock()->getPosition(), geometry);
    }
    
    static void registerMousePositionCallback(GLFWwindow* window, std::weak_ptr<Shape> shape, std::weak_ptr<Shape> geometry, glm::vec3 normal) {
        LineDrawer::normal = normal;
        registerMousePositionCallback(window, shape.lock()->getPosition(), geometry);
    }
    
    static void lineDrawerPositionCallback(GLFWwindow* window, double mousePosX, double mousePosY) {
        glm::vec3 newPosition = LineDrawer::computeNewLocation(mousePosX, mousePosY, lineData.startPosition);
        lineData.endPosition = newPosition;
        if (newPosition == lineData.startPosition) {
            return;
        }
        lineData.geometry.lock()->setModelingTransform(vector::scaleGeometryBetweenTwoPointsTransformation(newPosition, lineData.startPosition));
        lineData.geometry.lock()->onDrag();
    }
    
    static LineData getMostRecentLineData() {
        return lineData;
    }
    
    LineDrawer(Camera* camera) {
        MeshDragger::camera = camera;
    }
};

class MousePicker {
    friend LineDrawer;
public:
    
    MousePicker(Renderer* renderer, Camera* camera, Scene* theScene, std::function<void(double,double)> clickCustomization, std::function<void(double,double)> rightClickCustomization = [](double x, double y){}) {
        MousePicker::renderer = renderer;
        MousePicker::camera = camera;
        MousePicker::theScene = theScene;
        MousePicker::clickCustomization = clickCustomization;
        MousePicker::rightClickCustomization = rightClickCustomization;
    }
    
    void setClickCustomization(std::function<void(double,double)> clickCustomization) {
        this->clickCustomization = clickCustomization;
    }
    
    void setRightClickCustomization(std::function<void(double, double)> rightClickCustomization) {
        this->rightClickCustomization = rightClickCustomization;
    }
    
    virtual void enable(GLFWwindow* window) {
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        mousePositionCallback = mouse_position_callback;
        glfwSetCursorPosCallback(window, mousePositionCallback);
    }
    
    virtual void enableRayTrianglePicker(GLFWwindow* window) {
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        mousePositionCallback = ray_triangle_callback;
        glfwSetCursorPosCallback(window, mousePositionCallback);
    }
    
    static Ray computeMouseRay(int mousePosX, int mousePosY) {
        float x = (2.0f * mousePosX) / ScreenHeight::screen_width - 1.0f;
        float y = 1.0f - (2.0f * mousePosY) / ScreenHeight::screen_height;
        float z = 1.0f;
        glm::vec3 ray_nds = glm::vec3(x, y, z);
        //homogeneous clip coordinates
        glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0f, 1.0f);
        //invert the projection matrix to get the viewing coordinates, and as point
        glm::vec4 ray_eye = glm::inverse(renderer->getProjectionTransform()) * ray_clip;
        ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);
        //now finally back to world coordinates
        glm::vec4 ray_world = glm::inverse(camera->viewingTransformation()) * ray_eye;
        Ray retval;
        retval.direction = glm::normalize(glm::vec3(ray_world.x, ray_world.y, ray_world.z));
        retval.origin = camera->getPosition();
        return retval;
    }
    
private:
    
    struct MouseRayCollision {
        std::shared_ptr<Shape> targetShape{};
        glm::vec3 exactPosition{};
    };
    
    static void resetCollisionData(MouseRayCollision& collision) {
        collision.targetShape.reset();
        collision.exactPosition = glm::vec3(0.0f,0.0f,0.0f);
    }
    
    static Renderer* renderer;
    static Camera* camera;
    static Scene* theScene;
    static int mousePositionX;
    static int mousePositionY;
    static Ray ray;
    static MouseRayCollision collisionData;
    static std::shared_ptr<Shape> currentlySelectedShape;
    static std::function<void(double,double)> clickCustomization;
    static std::function<void(double,double)> rightClickCustomization;
    static void (*mousePositionCallback)(GLFWwindow*, double, double);
    
    static void computeWorldRay() {
        Ray mouseRay = computeMouseRay(mousePositionX, mousePositionY);
        MousePicker::ray.direction = mouseRay.direction;
        MousePicker::ray.origin = mouseRay.origin;
    }
    
    static std::vector<std::tuple<std::shared_ptr<Shape>, glm::vec3>> computeRayTriangleCollisions() {
        std::vector<std::tuple<std::shared_ptr<Shape>, glm::vec3>> candidates;
        std::vector<std::shared_ptr<Shape>> sceneItems = theScene->get();
        for (auto&& shape : sceneItems) {
            constexpr float epsilon = std::numeric_limits<float>::epsilon();
            std::vector<glm::vec3> triangles = shape->getPositions();
            for (int i = 0; i < triangles.size(); i+=3) {
                Triangle triangle(triangles[i], triangles[i+1], triangles[i+2]);
                auto result = vector::rayTriangleIntersection(ray, triangle);
                for (auto res : result) {
                    candidates.push_back(std::make_tuple(shape, res));
                }
            }
        }
        return candidates;
    }
    
    static std::vector<std::tuple<std::shared_ptr<Shape>, glm::vec3>> computeCollisions() {
        std::vector<std::tuple<std::shared_ptr<Shape>, glm::vec3>> candidates;
        std::vector<std::shared_ptr<Shape>> sceneItems = theScene->get();
        for (auto&& shape : sceneItems) {
            std::vector<glm::vec3> position = shape->getAABB();
            std::sort(position.begin(), position.end(), [](const glm::vec3& a, const glm::vec3& b) {
                if (a.x != b.x) {
                    return a.x < b.x; // Sort by x coordinate
                } else if (a.y != b.y) {
                    return a.y < b.y; // If x coordinates are equal, sort by y coordinate
                } else {
                    return a.z < b.z; // If both x and y coordinates are equal, sort by z coordinate
                }
            });
            glm::vec3 minB = position.front();
            glm::vec3 maxB = position.back();
            int NUMDIM = 3;
            //thanks graphics gems
            bool inside = true;
            char quadrant[NUMDIM];
            int i;
            int whichPlane;
            double maxT[NUMDIM];
            double candidatePlane[NUMDIM];
            glm::vec3 coord;    //hit point
            int LEFT = 1; int RIGHT = 2; int MIDDLE = 0;

            /* Find candidate planes; this loop can be avoided if
               rays cast all from the eye(assume perpsective view) */
            for (i=0; i<NUMDIM; i++) {
                if(ray.origin[i] < minB[i]) {
                    quadrant[i] = LEFT;
                    candidatePlane[i] = minB[i];
                    inside = false;
                }else if (ray.origin[i] > maxB[i]) {
                    quadrant[i] = RIGHT;
                    candidatePlane[i] = maxB[i];
                    inside = false;
                } else    {
                    quadrant[i] = MIDDLE;
                }
            }

            /* Ray origin inside bounding box */
            if(inside)    {
                coord = ray.origin;
                candidates.push_back(std::make_tuple(shape, coord));
                continue;
            }


            /* Calculate T distances to candidate planes */
            for (i = 0; i < NUMDIM; i++) {
                if (quadrant[i] != MIDDLE && ray.direction[i] != 0.)
                    maxT[i] = (candidatePlane[i]-ray.origin[i]) / ray.direction[i];
                else
                    maxT[i] = -1.;
            }

            /* Get largest of the maxT's for final choice of intersection */
            whichPlane = 0;
            for (i = 1; i < NUMDIM; i++) {
                if (maxT[whichPlane] < maxT[i])
                    whichPlane = i;
            }

            /* Check final candidate actually inside box */
            if (maxT[whichPlane] < 0.) {
                continue;
            }
            bool bHit = true;
            for (i = 0; i < NUMDIM; i++) {
                if (whichPlane != i) {
                    coord[i] = ray.origin[i] + maxT[whichPlane] * ray.direction[i];
                    if (coord[i] < minB[i] || coord[i] > maxB[i]) {
                        bHit = false;
                        shape->offHover();
                    }
                } else {
                    coord[i] = candidatePlane[i];
                }
            }
            if (bHit) {
                candidates.push_back(std::make_tuple(shape, coord));/* ray hits box */
            }
        }
        return candidates;
    }
    
    static void init(GLFWwindow* window) {
        mousePositionCallback = mouse_position_callback;
        glfwSetCursorPosCallback(window, mousePositionCallback);
    }
    
    static void initTrianglePicker(GLFWwindow* window) {
        mousePositionCallback = ray_triangle_callback;
        glfwSetCursorPosCallback(window, mousePositionCallback);
    }
    
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            if (collisionData.targetShape) {
                currentlySelectedShape = collisionData.targetShape;
                collisionData.targetShape->onClick(collisionData.exactPosition);
            }
            else {
                clickCustomization(mousePositionX, mousePositionY);
            }
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
            glfwSetCursorPosCallback(window, mousePositionCallback);
            if (currentlySelectedShape) {
                currentlySelectedShape->onMouseUp();
                currentlySelectedShape.reset();
            }
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
            if (collisionData.targetShape) {
                currentlySelectedShape = collisionData.targetShape;
                collisionData.targetShape->onRightClick();
            }
            else {
                rightClickCustomization(mousePositionX, mousePositionY);
            }
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
            glfwSetCursorPosCallback(window, mousePositionCallback);
            if (currentlySelectedShape) {
                currentlySelectedShape->onRightClickUp();
                currentlySelectedShape.reset();
            }
        }
    }
    
    static void ray_triangle_callback(GLFWwindow* window, double xpos, double ypos) {
        mousePositionX = xpos;
        mousePositionY = ypos;
        computeWorldRay();
        std::vector<std::tuple<std::shared_ptr<Shape>, glm::vec3>> candidates = computeRayTriangleCollisions();
        std::sort(candidates.begin(), candidates.end(), [](auto e, auto e2) {
            glm::vec4 res1 = camera->viewingTransformation() * glm::vec4(std::get<1>(e), 1.0f);
            glm::vec4 res2 = camera->viewingTransformation() * glm::vec4(std::get<1>(e2), 1.0f);
            return res2.z < res1.z;
        });
        if (candidates.size() == 0) {
            resetCollisionData(collisionData);
            return;
        }
        collisionData.targetShape = std::get<0>(candidates.front());
        collisionData.exactPosition = std::get<1>(candidates.front());
    }
    
    static void mouse_position_callback(GLFWwindow* window, double xpos, double ypos) {
        mousePositionX = xpos;
        mousePositionY = ypos;
        computeWorldRay();
        std::vector<std::tuple<std::shared_ptr<Shape>, glm::vec3>> candidates = computeCollisions();
        if (candidates.empty()) {
            collisionData.targetShape.reset();
            return;
        }
        //select the closest shape to the camera
        std::sort(candidates.begin(), candidates.end(), [](auto a, auto b) {
            glm::vec3 coordA = std::get<1>(a);
            glm::vec4 tmpA = glm::vec4(coordA.x,coordA.y,coordA.z, 1.0f);
            tmpA = camera->arcballTransformation() * tmpA;
            coordA = glm::vec3(tmpA.x,tmpA.y,tmpA.z);
            glm::vec3 coordB = std::get<1>(b);
            glm::vec4 tmpB = glm::vec4(coordB.x,coordB.y,coordB.z, 1.0f);
            tmpB = camera->arcballTransformation() * tmpB;
            coordB = glm::vec3(tmpB.x,tmpB.y,tmpB.z);
            if (coordA.z != coordB.z) {
                return coordA.z < coordB.z; // Sort by z coordinate
            } else if (coordA.y != coordB.y) {
                return coordA.y < coordB.y; // If z coordinates are equal, sort by y coordinate
            } else {
                return coordA.x < coordB.x; // If both z and y coordinates are equal, sort by x coordinate
            }
        });
        collisionData.targetShape = std::get<0>(candidates.back());
        collisionData.exactPosition = std::get<1>(candidates.back());
        collisionData.targetShape->onHover();
        candidates.pop_back();
        for (auto elt : candidates) {
            std::shared_ptr<Shape> shape = std::get<0>(elt);
            shape->offHover();
        }
    }
    
};


#endif /* mouse_h */
