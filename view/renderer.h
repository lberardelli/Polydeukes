//
//  renderer.h
//  LearnOpenGLProject1
//
//  Created by Lawrence Berardelli on 2024-02-26.
//

#ifndef renderer_h
#define renderer_h

#include "../model/cube.h"
#include "../control/ShaderProgram.h"
#include "../model/light.hpp"
#include "../model/Scene.h"
#include "../model/shape.h"
#include "../model/Camera.h"
#include "../model/cube.h"
#include "../model/square.h"
#include "../model/particle.h"
#include "../model/sphere.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <chrono>
#include <thread>
#include <algorithm>
#include <chrono>
#include <thread>
#include <stack>
#include <random>


class Renderer {
    
private:
    
    /*
     TODO: Maybe instead of a scene here we need a list of render data packages?
     Can sort by most to least expensive context switches.
     https://gamedev.stackexchange.com/questions/125546/what-is-the-best-way-to-handle-the-actual-render-loop
     */
    Scene* theScene;
    std::vector<Particle> particles{};
    glm::mat4 view;
    glm::mat4 projection = glm::perspective(glm::radians(fov), (float)screen_width / (float)screen_height, 0.1f, 100.0f);
    std::function<void()> preRenderCustomization = [] {};
    static float fov;
    
    struct RenderPackage {
        std::shared_ptr<Shape> shape;
        ShaderProgram* program;
    };
    
    ShaderProgram* defaultProgram;
    std::vector<RenderPackage> instructions{};
    
    bool areColliding(std::vector<glm::vec3> aabb1, std::vector<glm::vec3> aabb2) {
        // Check overlap on x-axis
        if (aabb1[1].x < aabb2[0].x || aabb1[0].x > aabb2[1].x) {
            return false;
        }

        // Check overlap on y-axis
        if (aabb1[1].y < aabb2[0].y || aabb1[0].y > aabb2[1].y) {
            return false;
        }

        // Check overlap on z-axis
        if (aabb1[1].z < aabb2[0].z || aabb1[0].z > aabb2[1].z) {
            return false;
        }

        // If there is overlap on all axes, collision is detected
        return true;
    }
    
    struct collision {
        Particle* first;
        Particle* second;
        
        collision(Particle* first, Particle* second) : first(first), second(second) {}
    };
  
public:
    
    static unsigned int screen_width;
    static unsigned int screen_height;
    
    Renderer(Scene* theScene, ShaderProgram* defaultProgram): defaultProgram(defaultProgram), theScene(theScene) {}
    
    void addPreRenderCustomization(std::function<void()> customization) {
        this->preRenderCustomization = customization;
    }
    
    glm::mat4 getViewingTransform() {
        return view;
    }
    
    glm::mat4 getProjectionTransform() {
        return projection;
    }
    
    std::shared_ptr<Shape> getShape(Shape* shape) {
        for (auto&& theshape : theScene->get()) {
            if (theshape.get() == shape) {
                return theshape;
            }
        }
        return std::shared_ptr<Shape>(nullptr);
    }
    
    void addMesh(std::shared_ptr<Shape> shape, ShaderProgram* program) {
        RenderPackage package{shape, program};
        instructions.push_back(package);
        theScene->addMesh(shape);
    }
    
    void addMesh(std::shared_ptr<Shape> shape) {
        addMesh(shape, defaultProgram);
    }
    
    void removeShape(std::shared_ptr<Shape> shape) {
        theScene->removeMesh(shape);
        auto it = std::remove_if(instructions.begin(), instructions.end(), [&](RenderPackage& package) {
            return package.shape == shape;
        });
        std::vector<Particle> tmp{};
        for (auto particle : particles) {
            if (particle.getShape() != shape) {
                tmp.push_back(particle);
            }
        }
        particles = tmp;
        instructions = std::vector<RenderPackage>(instructions.begin(), it);
    }
    
    void addParticle(Particle particle) {
        addParticle(particle, defaultProgram);
    }
    
    void addParticle(Particle particle, ShaderProgram* program) {
        addMesh(particle.getShape(), program);
        particles.push_back(particle);
    }
    
    bool isOutsideBoundary(Particle& particle) {
        return glm::length(particle.getPosition()) > 50;
    }
    
    /*
     TODO: Need to couple the shading program and the object in the scene
     This is related to the rendering data package as in the shading program could be in the rendering package.
     Though that would lead to a bind call on the shading program for every render call. In fact, that is
     bad according to top answer https://www.reddit.com/r/opengl/comments/3etkgc/performance_costs_of_switching_shaders_between/
     so we will need an appropriate solution, probably not switching shaders at all until necessary.
     
     TODO: Need to sort the items of the scene to minimize context switching. 
     */
    void buildandrender(GLFWwindow* window, Camera* camera, Scene* theScene) {
        this->theScene = theScene;
        defaultProgram->setInt("texture1", 0);
        light light(glm::vec3(1.0,1.0,1.0), glm::vec3(0.0, 5.0, 4.0));
        addMesh(CubeBuilder().withPosition(light.position).withColour(glm::vec3(1.0f,1.0f,1.0f)).build());
        ShaderProgram* previousProgram = 0;
        double framerate = 60;
        int i = 1;
        SphereBuilder::getInstance()->withPosition(glm::vec3(-10.0f,0.0f,0.0f)).withColour(glm::vec3(1.0f,1.0f,1.0f));
        auto start = std::chrono::high_resolution_clock::now();
        while (!glfwWindowShouldClose(window)) {
            preRenderCustomization();
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            view = camera->viewingTransformation();
            glm::vec3 cameraPosition = camera->getPosition();
            for (RenderPackage package : instructions) {
                package.program->bind();
                package.program->setMat4("view", view);
                package.program->setMat4("projection", projection);

                //set the lighting uniforms
                //program.setVec3("aColour", glm::vec3(1.0f, 0.5f, 0.31f));
                package.program->setVec3("lightColour", light.colour);
                package.program->setVec3("lightPosition", light.position);
                package.program->setVec3("eye", cameraPosition);
                previousProgram = package.program;
                package.shape->render(*package.program);
            }
            std::vector<collision> collisions{};
            for (int i = 0; i < particles.size(); ++i) {
                for (int j = i; j < particles.size(); ++j) {
                    if (areColliding(particles[i].getShape()->getAABB(), particles[j].getShape()->getAABB())) {
                        particles[i].basicCollision();
                        particles[j].basicCollision();
                    }
                }
            }
            
            for (Particle& particle : particles) {
                particle.update(float(1)/(float) framerate);
            }
            
            for (std::vector<Particle>::iterator it = particles.begin(); it != particles.end();) {
                if (isOutsideBoundary(*it)) {
                    removeShape(it->getShape());
                }
                else {
                    ++it;
                }
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration = end - start;
            double frameTime = duration.count();
            if (frameTime < (1.f/60.f)) {
                std::chrono::duration<double> sleepTime((1.f/60.f) - frameTime);
                std::this_thread::sleep_for(sleepTime);
            }
            if (i % 10 == 0) {
                std::cout << "framerate: " << 1.f/frameTime << std::endl;
                i = 0;
            }
            ++i;
            start = std::chrono::high_resolution_clock::now();
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
    
};

#endif /* renderer_h */
