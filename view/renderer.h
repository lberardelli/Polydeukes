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

class Chip8InputHandler {
    static const int constexpr key_codes[16] = {
        305, 306, 307, 308, 282, 336, 331, 332, 327, 328, 329, 333, 324, 325, 326, 334
    };
    
    static bool is_key_pressed[16];
    
    static int isKeyOfInterest(int key) {
        for (int i = 0; i < 16; ++i) {
            if (key == key_codes[i]) {
                return i;
            }
        }
        return -1;
    }
    
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        int key_code_index = isKeyOfInterest(key);
        if (key_code_index >= 0) {
            if (action == GLFW_PRESS) {
                std::cout << "Key Pressed: " << key << std::endl;
                is_key_pressed[key_code_index] = true;
            } else if (action == GLFW_RELEASE) {
                std::cout << "Key Released: " << key << std::endl;
                is_key_pressed[key_code_index] = false;
            }
        }
    }
    
public:
    
    explicit Chip8InputHandler(GLFWwindow* window) {
        glfwSetKeyCallback(window, key_callback);
    }
    
    bool isKeyPressed(int index) {
        return is_key_pressed[index];
    }
};

class Chip8Interpreter {
    std::array<unsigned int, 4096> ram{};
    unsigned int programCounter{};
    unsigned int indexRegister{};
    std::stack<unsigned int> stack{};
    unsigned int soundTimer{};
    unsigned int delayTimer{};
    std::array<unsigned int, 16> registers{};
    std::array<std::shared_ptr<Shape>, 2048>* display{};
    Chip8InputHandler* inputHandler;
    
public:
    
    Chip8Interpreter() {}
    
    Chip8Interpreter(std::array<unsigned int, 4096>& ram,
                 unsigned int programCounter,
                 unsigned int indexRegister,
                 std::stack<unsigned int>& stack,
                 unsigned int soundTimer,
                 unsigned int delayTimer,
                 std::array<unsigned int, 16>& registers,
                 std::array<std::shared_ptr<Shape>, 2048>* display,
                     Chip8InputHandler* handler)
            :   ram(ram),
                programCounter(programCounter),
                indexRegister(indexRegister),
                stack(stack),
                soundTimer(soundTimer),
                delayTimer(delayTimer),
                registers(registers),
                display(display), inputHandler(handler)
        {
        }
    
    void fetchDecodeExecute() {
        for (int i = 0; i < 10; ++i) {
            unsigned int instruction = (ram[programCounter] << 8) | ram[programCounter + 1];
            programCounter += 2;
            if (instruction == 0x00E0) {
                for (auto pixel : *display) {
                    pixel->setColour(glm::vec3(0.0f,0.0f,0.0f));
                }
            }
            else if (instruction == 0x00EE) {
                programCounter = stack.top();
                stack.pop();
            }
            else if ((instruction & 0xF000) == 0x1000) {
                programCounter = (instruction & 0xFFF);
            }
            else if ((instruction & 0xF000) == 0x2000) {
                unsigned int memoryLocation = instruction & 0xFFF;
                stack.push(programCounter);
                programCounter = memoryLocation;
            }
            else if ((instruction & 0xF000) == 0x3000) {
                unsigned int registerValue = registers[((instruction & 0xF00) >> 8)];
                if (registerValue == (instruction & 0xFF)) {
                    programCounter += 2;
                }
            }
            else if ((instruction & 0xF000) == 0x4000) {
                unsigned int registerValue = registers[((instruction & 0xF00) >> 8)];
                if (registerValue != (instruction & 0xFF)) {
                    programCounter += 2;
                }
            }
            else if ((instruction & 0xF000) == 0x5000) {
                unsigned int registerValue = registers[((instruction & 0xF00) >> 8)];
                unsigned int registerValue2 = registers[((instruction & 0xF0) >> 4)];
                if (registerValue == registerValue2) {
                    programCounter += 2;
                }
            }
            else if ((instruction & 0xF000) == 0x6000) {
                registers[(instruction & 0xF00) >> 8] = instruction & 0xFF;
            }
            else if ((instruction & 0xF000) == 0x7000) {
                registers[(instruction & 0xF00) >> 8] = (registers[(instruction & 0xF00) >> 8] + (instruction & 0xFF)) % 256;
            }
            else if ((instruction & 0xF000) == 0x8000) {
                if ((instruction & 0xF) == 0) {
                    registers[((instruction & 0xF00) >> 8)] = registers[((instruction & 0xF0) >> 4)];
                }
                else if ((instruction & 0xF) == 1) {
                    registers[((instruction & 0xF00) >> 8)] = registers[((instruction & 0xF00) >> 8)] | registers[((instruction & 0xF0) >> 4)];
                }
                else if ((instruction & 0xF) == 2) {
                    registers[((instruction & 0xF00) >> 8)] = registers[((instruction & 0xF00) >> 8)] & registers[((instruction & 0xF0) >> 4)];
                }
                else if ((instruction & 0xF) == 3) {
                    registers[((instruction & 0xF00) >> 8)] = registers[((instruction & 0xF00) >> 8)] ^ registers[((instruction & 0xF0) >> 4)];
                }
                else if ((instruction & 0xF) == 4) {
                    registers[0xF] = 0;
                    unsigned int result = registers[((instruction & 0xF00) >> 8)] + registers[((instruction & 0xF0) >> 4)];
                    if (result > 255) {
                        registers[0xF] = 1;
                    }
                    registers[((instruction & 0xF00) >> 8)] = result % 256;
                }
                else if ((instruction & 0xF) == 5) {
                    unsigned int x = (instruction & 0xF00) >> 8;
                    unsigned int y = (instruction & 0xF0) >> 4;
                    if (registers[x] >= registers[y]) {
                        registers[0xF] = 1; // No borrow
                        registers[x] = registers[x] - registers[y];
                    } else {
                        registers[0xF] = 0; // Borrow occurred
                        registers[x] = (registers[x] - registers[y] + 256) % 256;
                    }
                }
                else if ((instruction & 0xF) == 7) {
                    unsigned int x = (instruction & 0xF00) >> 8;
                    unsigned int y = (instruction & 0xF0) >> 4;
                    if (registers[y] >= registers[x]) {
                        registers[0xF] = 1; // No borrow
                        registers[x] = registers[y] - registers[x];
                    } else {
                        registers[0xF] = 0; // Borrow occurred
                        registers[x] = (registers[y] - registers[x] + 256) % 256;
                    }
                }
                else if ((instruction & 0xF) == 6) {
                    registers[((instruction & 0xF00) >> 8)] = registers[((instruction & 0xF0) >> 4)];
                    registers[0xF] = registers[((instruction & 0xF00) >> 8)] & 1;
                    registers[((instruction & 0xF00) >> 8)] = registers[((instruction & 0xF00) >> 8)] >> 1;
                }
                else if ((instruction & 0xF) == 14) {
                    registers[((instruction & 0xF00) >> 8)] = registers[((instruction & 0xF0) >> 4)];
                    registers[0xF] = ((registers[((instruction & 0xF00) >> 8)] & 0xF000) >> 15);
                    registers[((instruction & 0xF00) >> 8)] = registers[((instruction & 0xF00) >> 8)] << 1;
                }
            }
            else if ((instruction & 0xF000) == 0x9000) {
                unsigned int registerValue = registers[((instruction & 0xF00) >> 8)];
                unsigned int registerValue2 = registers[((instruction & 0xF0) >> 4)];
                if (registerValue != registerValue2) {
                    programCounter += 2;
                }
            }
            else if ((instruction & 0xF000) == 0xA000) {
                indexRegister = instruction & 0xFFF;
            }
            else if ((instruction & 0xF000) == 0xB000) {
                programCounter = (instruction & 0xFFF) + registers[0];
            }
            else if ((instruction & 0xF000) == 0xC000) {
                std::random_device dev;
                std::mt19937 rng(dev());
                std::uniform_int_distribution<std::mt19937::result_type> dist(0,4294967295);
                unsigned int number = dist(rng);
                registers[((instruction & 0xF00) >> 8)] = ((instruction & 0xFF) & number);
            }
            else if ((instruction & 0xF000) == 0xD000) {
                unsigned int horizontalCoordinate = (registers[(instruction & 0xF00) >> 8] & 63);
                unsigned int verticalCoordinate = registers[(instruction & 0xF0) >> 4] & 31;
                unsigned int indexOfFirstPixel = verticalCoordinate * 64 + horizontalCoordinate;
                unsigned int memoryLocation = indexRegister;
                registers[0xF] = 0;
                int height = (instruction & 0xF);
                for (int k = 0; k < height; ++k) {
                    unsigned int sprite = ram[memoryLocation + k];
                    for (int i = 0; i < 8; ++i) {
                        if ((sprite & 1) == 1) {
                            if (horizontalCoordinate + 8 - i > 63) {
                                continue;
                            }
                            auto pixel = (*display)[indexOfFirstPixel + 7 - i];
                            if (pixel->getColour().x > 0.5) {
                                registers[0xF] = 1;
                                pixel->setColour(glm::vec3(0.f,0.f,0.f));
                            }
                            else {
                                pixel->setColour(glm::vec3(1.0f,1.0f,1.0f));
                            }
                        }
                        sprite >>= 1;
                    }
                    indexOfFirstPixel += 64;
                    if (verticalCoordinate + k + 1 > 31) {
                        break;
                    }
                }
            }
            else if ((instruction & 0xF000) == 0xE000) {
                if ((instruction & 0xFF) == 0x9E) {
                    int keyIndex = registers[((instruction & 0xF00) >> 8)];
                    if (inputHandler->isKeyPressed(keyIndex)) {
                        programCounter += 2;
                    }
                }
                else if ((instruction & 0xFF) == 0xA1) {
                    int keyIndex = registers[((instruction & 0xF00) >> 8)];
                    if (!inputHandler->isKeyPressed(keyIndex)) {
                        programCounter += 2;
                    }
                }
            }
            else if ((instruction & 0xF000) == 0xF000) {
                if ((instruction & 0xFF) == 0x7) {
                    registers[((instruction & 0xF00) >> 8)] = delayTimer;
                }
                else if ((instruction & 0xFF) == 0x15) {
                    delayTimer = registers[((instruction & 0xF00) >> 8)];
                }
                else if ((instruction & 0xFF) == 0x18) {
                    soundTimer = registers[((instruction & 0xF00) >> 8)];
                }
                else if ((instruction & 0xFF) == 0x1E) {
                    indexRegister += registers[((instruction & 0xF00) >> 8)];
                }
                else if ((instruction & 0xFF) == 0x0A) {
                    programCounter -= 2;
                    for (int i = 0; i < 16; ++i) {
                        if (inputHandler->isKeyPressed(i)) {
                            registers[((instruction & 0xF00) >> 8)] = i;
                            programCounter += 2;
                            break;
                        }
                    }
                }
                else if ((instruction & 0xFF) == 0x29) {
                    indexRegister = 5 * registers[((instruction & 0xF00) >> 8)];
                }
                else if ((instruction & 0xFF) == 0x33) {
                    int value = registers[((instruction & 0xF00) >> 8)];
                    ram[indexRegister] = value / 100;
                    value %= 100;
                    ram[indexRegister+1] = value / 10;
                    ram[indexRegister+2] = value % 10;
                }
                else if ((instruction & 0xFF) == 0x55) {
                    int index = ((instruction & 0xF00) >> 8);
                    for (int i = 0; i <= index; ++i) {
                        ram[indexRegister + i] = registers[i];
                    }
                }
                else if ((instruction & 0xFF) == 0x65) {
                    int index = ((instruction & 0xF00) >> 8);
                    for (int i = 0; i <= index; ++i) {
                        registers[i] = ram[indexRegister + i];
                    }
                }
            }
            if (delayTimer > 0) {
                --delayTimer;
            }
            if (soundTimer > 0) {
                --soundTimer;
            }
        }
    }
};


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
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)screen_width / (float)screen_height, 0.1f, 1000.0f);
    Chip8Interpreter interpreter{};
    bool runChip8 = false;
    
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
    
    void addChip8Interpreter(Chip8Interpreter&& interpreter) {
        this->interpreter = interpreter;
        runChip8 = true;
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
        light light(glm::vec3(1.0,1.0,1.0), glm::vec3(3.0, 5.0, 8.0));
        ShaderProgram* previousProgram = 0;
        double framerate = 60;
        int i = 1;
        SphereBuilder::getInstance()->withPosition(glm::vec3(-10.0f,0.0f,0.0f)).withColour(glm::vec3(1.0f,1.0f,1.0f));
        auto start = std::chrono::high_resolution_clock::now();
        while (!glfwWindowShouldClose(window)) {
            if (runChip8) {
                interpreter.fetchDecodeExecute();
            }
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
