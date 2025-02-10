#include <stdio.h>
#include "Arcball.h"

#include <iostream>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <cmath>

#include "../model/Camera.h"
#include "../view/screenheight.h"
#include "MouseController.h"

double Arcball::lastMouseX = 0;
double Arcball::lastMouseY = 0;
glm::vec3 Arcball::mouseDownVector = glm::vec3(0,0,1);
glm::quat Arcball::currentRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
glm::quat Arcball::previousRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
Camera* Arcball::cam{};
    
glm::vec3 Arcball::calculateVectorToCenterOfArcBall(double xpos, double ypos)
{
    glm::vec3 temp((xpos-ScreenHeight::screen_width/2)/(double)ScreenHeight::screen_height/2, (ypos-ScreenHeight::screen_height/2)/(double)ScreenHeight::screen_height/2, 0);
    float rad = temp.x * temp.x + temp.y * temp.y;
    if (rad > 1.0f) {
        float scale = 1.0/std::sqrtf(rad);
        temp = glm::vec3(temp.x * scale, temp.y * scale, 0.0f);
    }
    else {
        temp.z = std::sqrtf(1 - rad);
    }
    return glm::vec3(temp.x, -temp.y, temp.z);
}

/*
 No faces no names no body at all blindly guiding to the master house.
 */

void Arcball::arcball_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    //if you think about it, the plane created by the prev vector and the cur vector has normal which is the euler axis for the arcball rotation.
    //the angle is then the dot product, or since they will be normalized, the cosine of the angle between the two vectors.
    //that's enough to start using quaternions
    glm::vec3 currentPositionVector = calculateVectorToCenterOfArcBall(xpos, ypos);
    
    glm::vec3 norm = glm::cross(glm::normalize(mouseDownVector), glm::normalize(currentPositionVector));
    if (glm::length(norm) > 0.000001f) {
        float cosine = glm::dot(mouseDownVector, currentPositionVector);
        currentRotation = glm::quat(cosine, norm.x, norm.y, norm.z) * previousRotation;
        cam->updateViewingTransformation(glm::mat4_cast(currentRotation));
    }
}

void Arcball::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    cam->zoomIn(yoffset);
}

void Arcball::null_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    lastMouseX = xpos;
    lastMouseY = ypos;
}

void Arcball::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        //register cursor location callback
        mouseDownVector = calculateVectorToCenterOfArcBall(lastMouseX, lastMouseY);
        previousRotation = currentRotation;
        glfwSetCursorPosCallback(window, arcball_position_callback);
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        //unregister cursor location callback
        glfwSetCursorPosCallback(window, null_position_callback);
    }
}
    
/*
 This is a static class to accommodate the glfw callback registration api... could seperate that in future.
 */
void Arcball::setup(GLFWwindow* window, Camera* cam) {
    lastMouseX = 0;
    lastMouseY = 0;
    glfwSetCursorPosCallback(window, null_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    Arcball::cam = cam;
}

Arcball::Arcball(Camera* camera) {
    cam = camera;
}

void Arcball::enable(GLFWwindow* window) {
    setup(window, cam);
}

void Arcball::registerRotationCallback(GLFWwindow* window, int xpos, int ypos) {
    mouseDownVector = calculateVectorToCenterOfArcBall(xpos, ypos);
    previousRotation = currentRotation;
    glfwSetCursorPosCallback(window, arcball_position_callback);
}
