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
#include "../model/Camera.h"
#include "../model/Scene.h"
#include "../model/shape.h"
#include "Arcball.h"
#include "vector.h"

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
    static std::shared_ptr<Shape> targetShape;
    
public:
    
    static Renderer* renderer;
    static Camera* camera;
    
    static void registerMousePositionCallback(GLFWwindow* window, std::shared_ptr<Shape> shape) {
        glfwSetCursorPosCallback(window, meshDraggerPositionCallback);
        targetShape = shape;
    }
    
    static void meshDraggerPositionCallback(GLFWwindow* window, double mousePosX, double mousePosY) {
        glm::vec3 newPosition = computeNewLocation(mousePosX, mousePosY);
        glm::vec3 delta = newPosition - targetShape->getPosition();
        targetShape->updateModellingTransform(glm::translate(glm::mat4(1.0f), delta));
    }
    
    static glm::vec3 computeNewLocation(double mousePosX, double mousePosY);
    
    static void setTargetShape(Shape* shape) {
        std::shared_ptr<Shape> result = renderer->getShape(shape);
        targetShape = result;
    }
    
    MeshDragger(Camera* camera, Renderer* renderer) {
        MeshDragger::camera = camera;
        MeshDragger::renderer = renderer;
    }
};

class MousePicker {
    
public:
    
    MousePicker(Renderer* renderer, Camera* camera, Scene* theScene, std::function<void(double,double)> clickCustomization) {
        MousePicker::renderer = renderer;
        MousePicker::camera = camera;
        MousePicker::theScene = theScene;
        MousePicker::clickCustomization = clickCustomization;
    }
    
    virtual void enable(GLFWwindow* window) {
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetCursorPosCallback(window, mouse_position_callback);
    }
    
    static Ray computeMouseRay(int mousePosX, int mousePosY) {
        float x = (2.0f * mousePosX) / Renderer::screen_width - 1.0f;
        float y = 1.0f - (2.0f * mousePosY) / Renderer::screen_height;
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
    
    static Renderer* renderer;
    static Camera* camera;
    static Scene* theScene;
    static int mousePositionX;
    static int mousePositionY;
    static Ray ray;
    static std::shared_ptr<Shape> targetShape;
    static std::function<void(double,double)> clickCustomization;
    
    static void computeWorldRay() {
        Ray mouseRay = computeMouseRay(mousePositionX, mousePositionY);
        MousePicker::ray.direction = mouseRay.direction;
        MousePicker::ray.origin = mouseRay.origin;
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
        glfwSetCursorPosCallback(window, mouse_position_callback);
    }
    
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            if (targetShape) {
                targetShape->onClick();
                MeshDragger::registerMousePositionCallback(window, targetShape);
            }
            else {
                clickCustomization(mousePositionX, mousePositionY);
            }
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
            glfwSetCursorPosCallback(window, mouse_position_callback);
            if (targetShape) {
                targetShape->onMouseUp();
            }
        }
    }
    
    static void mouse_position_callback(GLFWwindow* window, double xpos, double ypos) {
        mousePositionX = xpos;
        mousePositionY = ypos;
        computeWorldRay();
        std::vector<std::tuple<std::shared_ptr<Shape>, glm::vec3>> candidates = computeCollisions();
        if (candidates.empty()) {
            targetShape.reset();
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
        targetShape = std::get<0>(candidates.back());
        targetShape->onHover();
        candidates.pop_back();
        for (auto elt : candidates) {
            std::shared_ptr<Shape> shape = std::get<0>(elt);
            shape->offHover();
        }
    }
    
};


#endif /* mouse_h */
