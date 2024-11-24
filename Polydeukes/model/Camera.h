//
//  camera.h
//  LearnOpenGLProject1
//
//  Created by Lawrence Berardelli on 2023-11-25.
//

#ifndef camera_h
#define camera_h

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <cmath>

class Camera {
    
private:
    
    glm::vec3 position;
    glm::vec3 lookatPoint;
    glm::vec3 z_axis,x_axis,y_axis;
    glm::mat4 view;
    glm::mat4 canonicalTransform;
    glm::mat4 arcballTransform = glm::mat4(1.0f);
    
    void setupViewingTransform() {
        //lookat basis
        z_axis = glm::normalize(position - lookatPoint);
        x_axis = glm::normalize(glm::cross(glm::vec3(0.0f,1.0f,0.0f), z_axis));
        y_axis = glm::cross(z_axis, x_axis);
        //imagine we have mesh positions in camera coordinates.
        //To get them to canonical coordinates we need to translate the canonical frame to the camera frame then evaluate the points given in camera frame.
        //Camera to Canonical transform (rotate to align the axies then translate origin to camera location
        double m[16] = {x_axis[0],x_axis[1],x_axis[2],0,y_axis[0],y_axis[1],y_axis[2],0,z_axis[0],z_axis[1],z_axis[2],0,position[0],position[1],position[2],1};
        canonicalTransform = glm::make_mat4(m);
        view = glm::inverse(canonicalTransform);
    }
    
    void updatePosition(glm::vec3 position) {
        this->position = position;
        setupViewingTransform();
        view = glm::inverse(canonicalTransform) * arcballTransform;
    }
    
    void onLeftArrowPress(GLFWwindow* window) {
        updatePosition(glm::vec3(position.x - .5f, position.y, position.z));
    }
    
    void onRightArrowPress(GLFWwindow* window) {
        updatePosition(glm::vec3(position.x + .5f, position.y, position.z));
    }
    
    void onUpArrowPress(GLFWwindow* window) {
        updatePosition(glm::vec3(position.x, position.y, position.z+.5f));
    }
    
    void onDownArrowPress(GLFWwindow* window){
        updatePosition(glm::vec3(position.x, position.y, position.z-.5f));
    }
    
    void onWPress(GLFWwindow* window){
        updatePosition(glm::vec3(position.x, position.y+.5, position.z));
    }
    
    void onSPress(GLFWwindow* window){
        updatePosition(glm::vec3(position.x, position.y-.5f, position.z));
    }
    
    
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
            if (camera) {
                if (action == GLFW_PRESS) {
                    switch (key) {
                        case GLFW_KEY_LEFT:
                            camera->onLeftArrowPress(window);
                            break;
                        case GLFW_KEY_RIGHT:
                            camera->onRightArrowPress(window);
                            break;
                        case GLFW_KEY_UP:
                            camera->onUpArrowPress(window);
                            break;
                        case GLFW_KEY_DOWN:
                            camera->onDownArrowPress(window);
                            break;
                        case GLFW_KEY_W:
                            camera->onWPress(window);
                            break;
                        case GLFW_KEY_S:
                            camera->onSPress(window);
                            break;
                        default:
                            break;
                    }
            }
        }
    }

    
public:
  
    glm::mat4 viewingTransformation() const {
        return view;
    }
    
    void enableFreeCameraMovement(GLFWwindow* window) {
        glfwSetWindowUserPointer(window, this);
        glfwSetKeyCallback(window, keyCallback);
    }
    
    Camera(glm::vec3 position, glm::vec3 lookatPoint) : position(position), lookatPoint(lookatPoint) {
        setupViewingTransform();
    }
    
    void updateViewingTransformation(glm::mat4&& transform) {
        arcballTransform = transform;
        view = glm::inverse(canonicalTransform) * transform;
    }
    
    glm::vec3 getPosition() {
        glm::vec4 tmp = glm::vec4(position, 1.0f) * arcballTransform;
        return tmp;
    }
    
    glm::vec3 getUpVector() {
        return glm::vec4(y_axis, 0.0f) * arcballTransform;
    }
    
    glm::vec3 getDirection() {
        return glm::vec4(-z_axis, 0.0f) * arcballTransform;
    }
    
    glm::mat4 arcballTransformation() {
        return arcballTransform;
    }
    
    void zoomIn(float yoffset) {
        position += glm::normalize(getDirection()) * yoffset * .1f;
        updatePosition(position);
    }

};


#endif /* camera_h */
