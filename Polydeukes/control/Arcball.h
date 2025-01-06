#ifndef arcball_h
#define arcball_h
#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glm.hpp>
#include "../model/Camera.h"
#include "MouseController.h"

class Arcball : MouseController {
    
private:
    
    static double lastMouseX;
    static double lastMouseY;
    static glm::vec3 mouseDownVector;
    static glm::quat previousRotation;
    static glm::quat currentRotation;
    static Camera* cam;
    
    static glm::vec3 calculateVectorToCenterOfArcBall(double xpos, double ypos);
    
    static void arcball_position_callback(GLFWwindow* window, double xpos, double ypos);
    
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    
    static void null_position_callback(GLFWwindow* window, double xpos, double ypos);
    
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        
    static void setup(GLFWwindow* window, Camera* cam);
  
public:
    
    explicit Arcball(Camera* camera);
    
    virtual void enable(GLFWwindow* window) override;
    
    void registerRotationCallback(GLFWwindow* window, int xpos, int ypos);
    
    void resetArcball() {
        previousRotation = glm::quat(1.0f,0.0f,0.0f,0.f);
        currentRotation = glm::quat(1.0f,0.0f,0.0f,0.f);
        cam->updateViewingTransformation(glm::mat4_cast(currentRotation));
    }

};


#endif /* arcball_h */
