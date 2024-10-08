//
//  MeshSpawner.h
//  LearnOpenGLProject1
//
//  Created by Lawrence Berardelli on 2024-02-26.
//

#ifndef MeshSpawner_h
#define MeshSpawner_h

#include "../model/cube.h"
#include "../view/renderer.h"
#include "MouseController.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <optional>
#include <exception>

/*
 Not responsible for freeing CubeFactory or Renderer
 */
class MeshSpawner : public MouseController {
  
private:
    static CubeFactory* cubeFactory;
    static Renderer* renderer;
    static int nClicks;
    
    static void init(GLFWwindow* window) {
        glfwSetMouseButtonCallback(window, mouse_button_callback);
    }
    
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            std::shared_ptr<Shape> cube = cubeFactory->build();
            cube->setModelingTransform(glm::mat4(1.0f));
            //maybe better to add this straight to the model and update the renderer scene reference.
            renderer->addMesh(cube);
            ++nClicks;
        }
    }
    
public:
    
    MeshSpawner(CubeFactory* factory, Renderer* renderer) {
        cubeFactory = factory;
        this->renderer = renderer;
    }
    
    virtual void enable(GLFWwindow* window) override {
        init(window);
    }
};

#endif /* MeshSpawner_h */
