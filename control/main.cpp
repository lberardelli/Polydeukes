//
//  main.cpp
//  LearnOpenGLProject1
//
//  Created by Lawrence Berardelli on 2023-11-04.
//

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "sierpinski.h"
#include "texture.h"
#include "cube.h"
#include "ShaderProgram.h"
#include "../view/renderer.h"
#include "MeshSpawner.h"
#include "MousePicker.h"
#include "../model/Scene.h"
#include "../model/Camera.h"
#include "../model/cube.h"
#include "../model/square.h"
#include "../model/sphere.h"
#include "../model/particle.h"
#include "../model/axies.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <functional>

/*
 TODO: need an undo button.
 */
class KeyboardController {
private:
    
    static Renderer* renderer;
    static std::shared_ptr<Shape> targetShape;
    static ShapeBuilder* sphereBuilder;
    static ShapeBuilder* cubeBuilder;
    static ShapeBuilder* squareBuilder;
    
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
            targetShape = cubeBuilder->build();
            Particle particle = Particle(targetShape, .1f);
            particle.addTensor(&Gravity::update);
            renderer->addParticle(particle);
        }
        else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
            targetShape = squareBuilder->build();
            Particle particle = Particle(targetShape, .1f);
            particle.addTensor(&Gravity::update);
            renderer->addParticle(particle);
        }
        else if (key == GLFW_KEY_M && action == GLFW_PRESS) {
            targetShape = sphereBuilder->build();
            Particle particle = Particle(targetShape, .1f);
            particle.addTensor(&Gravity::update);
            renderer->addParticle(particle);
        }
        else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
            if (targetShape) {
                glm::vec3 translationVector(0.0f, -1.0f, 0.0f);
                glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translationVector);
                targetShape->translate(translationMatrix);
            }
        }
        else if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
            if (targetShape) {
                glm::vec3 translationVector(0.0f, 1.0f, 0.0f);
                glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translationVector);
                targetShape->translate(translationMatrix);
            }
        }
        else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
            if (targetShape) {
                glm::vec3 translationVector(-1.0f, 0.0f, 0.0f);
                glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translationVector);
                targetShape->translate(translationMatrix);
            }
        }
        else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
            if (targetShape) {
                glm::vec3 translationVector(1.0f, 0.0f, 0.0f);
                glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translationVector);
                targetShape->translate(translationMatrix);
            }
        }
        else if (key == GLFW_KEY_B && action == GLFW_PRESS) {
            if (targetShape) {
                glm::vec3 translationVector(0.0f, 0.0f, -1.0f);
                glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translationVector);
                targetShape->translate(translationMatrix);
            }
        }
        else if (key == GLFW_KEY_F && action == GLFW_PRESS) {
            if (targetShape) {
                glm::vec3 translationVector(0.0f, 0.0f, 1.0f);
                glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translationVector);
                targetShape->translate(translationMatrix);
            }
        }
        else if (key == GLFW_KEY_DELETE && action == GLFW_PRESS) {
            if (targetShape) {
                renderer->removeShape(targetShape);
                targetShape = 0;
            }
        }
        else if (key == GLFW_KEY_C && action == GLFW_PRESS) {
            if (targetShape) {
                std::shared_ptr<Shape> cpy = targetShape->clone();
                renderer->addMesh(cpy);
            }
        }
    }
  
public:
    
    static MeshSpawner* meshSpawner;
    static Arcball* arcball;
    static MousePicker* mousepicker;
    static GLFWwindow* window;
    
    KeyboardController(std::shared_ptr<Shape> initialTargetShape) {
        targetShape = initialTargetShape;
    }
    
    void init(Renderer* renderer) {
        glfwSetKeyCallback(window, key_callback);
        KeyboardController::renderer = renderer;
        sphereBuilder = &SphereBuilder::getInstance()->withOnClickCallback([&](Shape* shape){ this->setTargetShape(shape); })
        .withOnHoverCallback([](Shape* shape) { shape->setColour(glm::vec3(1.0f, 0.53f, 0.3f)); })
        .withOffHoverCallback([](Shape* shape) { shape->setColour(glm::vec3(1.0f, 1.0f,1.0f)); });
        cubeBuilder = new CubeBuilder();
        cubeBuilder = &cubeBuilder->withColour(glm::vec3(1.0f,1.0f,1.0f))
        .withOnClickCallback([&](Shape* shape){ this->setTargetShape(shape); })
        .withOnHoverCallback([](Shape* shape) { shape->setColour(glm::vec3(1.0f, 0.53f, 0.3f)); })
        .withOffHoverCallback([](Shape* shape) { shape->setColour(glm::vec3(1.0f, 1.0f,1.0f)); });
        squareBuilder = new SquareBuilder();
        squareBuilder = &squareBuilder->withColour(glm::vec3(1.0f,1.0f,1.0f))
        .withOnClickCallback([&](Shape* shape){ this->setTargetShape(shape); })
        .withOnHoverCallback([](Shape* shape) { shape->setColour(glm::vec3(1.0f, 0.53f, 0.3f)); })
        .withOffHoverCallback([](Shape* shape) { shape->setColour(glm::vec3(1.0f, 1.0f,1.0f)); });
        mousepicker->enable(window);
    }
    
    void setTargetShape(Shape* target) {
        //find the shape in the renderer
        std::shared_ptr<Shape> result = renderer->getShape(target);
        targetShape = result;
    }
    
};

Camera* MeshDragger::camera{};
std::shared_ptr<Shape> MeshDragger::targetShape{};
Renderer* MeshDragger::renderer{};


Arcball* KeyboardController::arcball{};
MeshSpawner* KeyboardController::meshSpawner{};
MousePicker* KeyboardController::mousepicker{};
GLFWwindow* KeyboardController::window{};
Renderer* KeyboardController::renderer{};
std::shared_ptr<Shape> KeyboardController::targetShape{};
ShapeBuilder* KeyboardController::sphereBuilder{};
ShapeBuilder* KeyboardController::cubeBuilder{};
ShapeBuilder* KeyboardController::squareBuilder{};

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aCol;\n"
    "layout (location = 2) in vec2 aTex; \n"
    "uniform mat4 transform;\n"
    "out vec4 ourColour;\n"
    "out vec4 ourPosition;\n"
    "out vec2 TexCoord;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "   ourPosition = vec4(aPos, 1.0);\n"
    "   ourColour = vec4(aCol,0.2);\n"
    "   TexCoord = vec2(aTex.x, aTex.y);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec4 ourColour;\n"
    "in vec4 ourPosition;\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D texture1;\n"
    "uniform sampler2D texture2;\n"
    "void main()\n"
    "{\n"
    "   vec2 scaledUpTexture = vec2(TexCoord.x * 4.0, TexCoord.y * 4.0);\n"
    "   FragColor = mix(texture(texture1, TexCoord), texture(texture2, scaledUpTexture), 0.2) * ourColour;\n"
    "}\n\0";


void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
}

void textureExample(GLFWwindow* window, unsigned int shaderProgram) {
    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left
    };
    
    unsigned int texture = texture::generateTexture("/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/Polydeukes/Polydeukes/assets/container.jpg");
    unsigned int texture2 = texture::generateFaceTexture();
    
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };
    
    unsigned int VAO,VBO,EBO;
    
    /*
     what's a vertex array object? not sure, but binding it then doing operations on bound vertex buffers puts
     the resulting operation on the bound vao.
     */
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    //set the indices for the texture units.
    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1);
    
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(trans));
        
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        trans = glm::mat4(1.0f);
        trans = glm::translate(trans, glm::vec3(-0.5f, 0.5f, 0.0f));
        float time = glfwGetTime();
        float scale = sin(time);
        trans = glm::scale(trans, glm::vec3(scale, scale, scale));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(trans));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void triangleExample(GLFWwindow* window, unsigned int shaderProgram) {
    //vertices of triangle
    float vertices[] = {
        //position          //colour
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f //top
    };
    
    //set up vertex array and buffer objects
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    //render loop
    while(!glfwWindowShouldClose(window))
    {
        processInput(window);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(shaderProgram);
        int offsetLocation = glGetUniformLocation(shaderProgram, "offset");
        glUniform1f(offsetLocation, 0.5);
        
//        float timeValue = glfwGetTime();
//        float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
//        int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColour");
//        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
        

        
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

unsigned int buildShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource) {
    //compile the vertex shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    //compile the fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    //linking the shader program
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::LINK_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    return shaderProgram;
}

SceneList getFloor() {
    std::shared_ptr<Shape> s1 = SquareBuilder().withColour(glm::vec3(0.9,0.9,0.9)).build();
    std::shared_ptr<Shape> s2 = SquareBuilder().withColour(glm::vec3(0.1,0.1,0.1)).build();
    s1->setModelingTransform(glm::rotate(glm::mat4(1.0f), 3.14159f/2.0f, glm::vec3(1.0f,0.0f,0.0f)));
    s2->setModelingTransform(glm::rotate(glm::mat4(1.0f), 3.14159f/2.0f, glm::vec3(1.0f,0.0f,0.0f)));
    std::shared_ptr<Shape> cur;
    std::vector<std::unique_ptr<Shape>> graph{};
    for (int i = 0; i < 30; ++i) {
        glm::vec3 translation = glm::vec3(0.0f,0.0f,(float)i);
        for (int j = 0; j < 30; ++j) {
            if (i % 2) {
                if (j % 2) {
                    cur = s1;
                }
                else {
                    cur = s2;
                }
            }
            else {
                if (j % 2) {
                    cur = s2;
                }
                else {
                    cur = s1;
                }
            }
            translation = glm::vec3((float)j-10.0f, -5.0f, translation.z);
            cur->translate(translation);
            graph.push_back(cur->clone());
            cur->setModelingTransform(glm::rotate(glm::mat4(1.0f), 3.14159f/2.0f, glm::vec3(1.0f,0.0f,0.0f)));
        }
    }
    SceneList sceneGraph(std::move(graph));
    return sceneGraph;
}

void renderBasicPhysicsPlayground(GLFWwindow* window) {
    ShaderProgram program("/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/Polydeukes/Polydeukes/shaders/vertexshader.glsl", "/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/Polydeukes/Polydeukes/shaders/fragmentshader.glsl");
    program.init();
    Camera camera(glm::vec3(0.0f,10.f,35.f), glm::vec3(0.0f,0.0f,0.0f));
    CubeFactory factory;
    Scene theScene{};
    Renderer renderer(&theScene,&program);
    MeshSpawner spawner = MeshSpawner(&factory, &renderer);
    Arcball arcball = Arcball(&camera);
    
    MousePicker picker = MousePicker(&renderer, &camera, &theScene, [&](double mosPosx, double mosPosy) {
        arcball.registerRotationCallback(window, mosPosx, mosPosy);
    });
    MeshDragger::camera = &camera;
    MeshDragger::renderer = &renderer;
    /*
     TODO: needing to pass the same camera to the renderer and the arcball is bad.
     Should be injecting a rendering data package to the renderer from the control as needed.
     I think that's gonna be coupling the arcball to a render data package in the renderer.
     
    */
    
    /*
     TODO: What I'm really trying to do here is create an animation studio package.
     I want something with creation mode, staging mode, and recording mode.
     Creation mode can draw meshes and create or load animations.
     Staging mode can organize meshes and lighting and camera positioning and such.
     Recording mode actually plays out the scene.
     */
    std::shared_ptr<Shape> secondFloor = SquareBuilder().withColour(glm::vec3(0.9,0.9,0.9)).build();
    KeyboardController controller(secondFloor);
    secondFloor->setOnClick([&](Shape* shape){
        controller.setTargetShape(shape);
    });
    KeyboardController::arcball = &arcball;
    KeyboardController::meshSpawner = &spawner;
    KeyboardController::mousepicker = &picker;
    KeyboardController::window = window;
    controller.init(&renderer);
    std::unique_ptr<Shape> cube = CubeBuilder().withOnHoverCallback([](Shape* shape) {
        shape->setColour(glm::vec3(1.0f, 0.53f, 0.3f));
    }).withOffHoverCallback([](Shape* shape) {
        shape->setColour(glm::vec3(1.0f, 1.0f, 1.0f));
    }).withOnClickCallback([&](Shape* shape){
        controller.setTargetShape(shape);
    }).build();
    std::shared_ptr<Shape> springCoil = CubeBuilder().withOnHoverCallback([](Shape* shape) { shape->setColour(glm::vec3(0.9803921568627451,0.7764705882352941,0.03137254901960784)); })
        .withOffHoverCallback([](Shape* shape) { shape->setColour(glm::vec3(1.0f,1.0f,1.0f)); }).build();
    std::shared_ptr<Shape> springCoilCopy = springCoil->clone();
    renderer.addMesh(springCoil); renderer.addMesh(springCoilCopy);
    Spring spring = Spring(0.9f,2, glm::vec3(3.0f,3.0f,0.0), springCoil);
    Spring cielingSpring = Spring(0.9f,1,glm::vec3(10.0f,10.0f,0.0f), springCoilCopy);
    Drag drag = Drag(0.1f,0.1f);
    std::unique_ptr<Shape> sphere = SphereBuilder::getInstance()->build();
    sphere->setModelingTransform(glm::scale(glm::mat4(1.0f), glm::vec3(0.5,0.5,0.5)));
    SceneListBuilder sceneGraphBuilder = SceneListBuilder();
    sceneGraphBuilder.withOnHoverCallback([](Shape* shape) {
        shape->setColour(glm::vec3(1.0f, 0.53f, 0.3f));
    }).withOffHoverCallback([](Shape* shape) {
        shape->setColour(glm::vec3(1.0f, 1.0f, 1.0f));
    }).withOnClickCallback([&](Shape* shape){
        controller.setTargetShape(shape);
    });
    std::unique_ptr<Shape> subGraph = sceneGraphBuilder.build();
    std::vector<std::unique_ptr<Shape>> tmp;
    tmp.push_back(cube->clone());
    tmp.push_back(AxiesBuilder().build());
    static_cast<SceneList*>(subGraph.get())->addShapes(std::move(tmp));
    static_cast<SceneList*>(subGraph.get())->setPosition(glm::vec3(2.0f,2.0f,2.0f));
    std::unique_ptr<Shape> otherGraph = sceneGraphBuilder.build();
    std::vector<std::unique_ptr<Shape>> tmp2;
    tmp2.push_back(std::move(sphere));
    tmp2.push_back(AxiesBuilder().build());
    static_cast<SceneList*>(otherGraph.get())->addShapes(std::move(tmp2));
    std::shared_ptr<Shape> springEndSpriteS = sceneGraphBuilder.build();
    std::shared_ptr<SceneList> springEndSprite = std::dynamic_pointer_cast<SceneList>(springEndSpriteS);
    std::vector<std::unique_ptr<Shape>> tmp3;
    tmp3.push_back(std::move(otherGraph));
    tmp3.push_back(std::move(subGraph));
    springEndSprite->addShapes(std::move(tmp3));
    springEndSprite->setPosition(glm::vec3(10.0f,20.0f,0.0f));
    glm::vec3 lookat = springEndSprite->getPosition() - spring.getFixedPosition();
    std::unique_ptr<Shape> square = SquareBuilder().build();
    square->updateModellingTransform(glm::scale(glm::mat4(1.0f), glm::vec3(6.0f,6.0f,6.0f)));
    std::shared_ptr<Shape> platformGraphS = sceneGraphBuilder.build();
    std::shared_ptr<SceneList> platformGraph = std::dynamic_pointer_cast<SceneList>(platformGraphS);
    std::vector<std::unique_ptr<Shape>> tmp4;
    tmp4.push_back(std::move(square));
    tmp4.push_back(AxiesBuilder().build());
    platformGraph->addShapes(std::move(tmp4));
    platformGraph->updateModellingTransform(vector::buildChangeOfBasisMatrix(lookat));
    platformGraph->updateModellingTransform(glm::translate(glm::mat4(1.0f), glm::vec3(3.0f,3.0f,0.0f)));
    std::shared_ptr<Shape> cielingS = sceneGraphBuilder.build();
    std::shared_ptr<SceneList> cieling = std::dynamic_pointer_cast<SceneList>(cielingS);
    std::vector<std::unique_ptr<Shape>> tmp5;
    tmp5.push_back(SquareBuilder().build());
    tmp5.push_back(AxiesBuilder().build());
    cieling->addShapes(std::move(tmp5));
    cieling->updateModellingTransform(glm::rotate(glm::mat4(1.0f), 3.141592f/2.0f, glm::vec3(1.0f,0.0f,0.0f)));
    cieling->updateModellingTransform(glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f,0.0f)));
    renderer.addParticle(Particle(cieling, 0));
    Particle springEnd(springEndSprite->clone(), 0.3333);
    springEnd.addTensor(std::bind(&Spring::update, &cielingSpring, std::placeholders::_1, std::placeholders::_2));
    renderer.addParticle(springEnd);
    Particle springHead(springEndSprite->clone(), 0.10);
    springHead.addTensor(std::bind(&Spring::update, &spring, std::placeholders::_1, std::placeholders::_2));
    springHead.addTensor(&Gravity::update);
    renderer.addParticle(springHead);
    renderer.addParticle(Particle(platformGraph, 0));
    secondFloor->setModelingTransform(glm::scale(glm::translate(glm::rotate(glm::mat4(1.0f), 3.14159f/2.0f, glm::vec3(1.0f,0.0f,0.0f)), glm::vec3(0.0f,0.0f,10.0f)), glm::vec3(100.0f,100.0f,100.0f)));
    std::shared_ptr<Shape> northWall = SquareBuilder().build();
    northWall->setModelingTransform(glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f,-400.0f)), glm::vec3(1000.0f,1000.0f,1000.0f)));
    std::shared_ptr<Shape> westWall = SquareBuilder().build();
    westWall->setModelingTransform(glm::scale(glm::translate(glm::rotate(glm::mat4(1.0f), 3.14159f/2.0f, glm::vec3(0.0f,1.0f,0.0f)), glm::vec3(0.0f,0.0f,400.0f)), glm::vec3(1000.0f,1000.0f,1000.0f)));
    std::shared_ptr<Shape> eastWall = SquareBuilder().build();
    eastWall->setModelingTransform(glm::scale(glm::translate(glm::rotate(glm::mat4(1.0f), 3.14159f/2.0f, glm::vec3(0.0f,-1.0f,0.0f)), glm::vec3(0.0f,0.0f,400.0f)), glm::vec3(1000.0f,1000.0f,1000.0f)));
    
    ShaderProgram
        gridProgram("/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/Polydeukes/Polydeukes/shaders/lightsourcevs.glsl", "/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/Polydeukes/Polydeukes/shaders/gridfs.glsl");
    gridProgram.init();
    
    renderer.addParticle(Particle(secondFloor, 0),&gridProgram);
    renderer.addParticle(Particle(northWall, 0),&gridProgram);
    renderer.addParticle(Particle(eastWall, 0),&gridProgram);
    renderer.addParticle(Particle(westWall, 0),&gridProgram);
    
    Particle pellet(SphereBuilder::getInstance()->build(), 0.10);
    pellet.addTensor(&Gravity::update);
    pellet.addTensor(std::bind(&Drag::update, &drag, std::placeholders::_1, std::placeholders::_2));
    renderer.addParticle(pellet);
    renderer.addMesh(CubeBuilder().build());
    renderer.buildandrender(window, &camera, &theScene);
}

void renderMotionCaptureScene(GLFWwindow* window) {
    ShaderProgram program("/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/Polydeukes/Polydeukes/shaders/vertexshader.glsl", "/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/Polydeukes/Polydeukes/shaders/fragmentshader.glsl");
    program.init();
    Camera camera(glm::vec3(0.,100.f,400.f), glm::vec3(0.f,100.f,0.f));
    Scene theScene{};
    Renderer renderer(&theScene,&program);
    Arcball arcball = Arcball(&camera);
    MousePicker picker = MousePicker(&renderer, &camera, &theScene, [&](double mosPosx, double mosPosy) {
        arcball.registerRotationCallback(window, mosPosx, mosPosy);
    });
    picker.enable(window);
    std::string bvhFile = "/Users/lawrenceberardelli/Downloads/cmuconvert-daz-60-75/63/63_05.bvh";
    std::shared_ptr<SceneGraph> graph(new SceneGraph(bvhFile));
    renderer.addMesh(std::dynamic_pointer_cast<Shape>(graph), &program);
    renderer.buildandrender(window, &camera, &theScene);
}

std::vector<glm::vec3> computeInterpolatingPolynomial(std::vector<std::shared_ptr<Shape>>& controlPoints, Renderer& renderer) {
    glm::mat4 constraintMatrix = glm::mat4(
            1.0f, 0.0f,   0.0f,     0.0f,
            1.0f, 1.0f/3.0f, 1.0f/9.0f, 1.0f/27.0f,
            1.0f, 2.0f/3.0f, 4.0f/9.0f, 8.0f/27.0f,
            1.0f, 1.0f,   1.0f,    1.0f
        );
    glm::mat4 blendingMatrix = glm::inverse(constraintMatrix);
    glm::mat4x3 result;
    for (int i = 0; i < 4; ++i) {
        glm::vec4 row = blendingMatrix[i];
        glm::vec3 accum = glm::vec3(0.0f,0.0f,0.0f);
        for (int j = 0; j < 4; ++j) {
             accum += row[j] * controlPoints[j]->getPosition();
        }
        result[i] = accum;
    }
    std::vector<glm::vec3> positions{};
    for (float i = 0.f; i < 1.00f; i += 0.001) {
        glm::vec3 value = glm::vec3(0.0f,0.0f,0.0f);
        glm::vec4 u = glm::vec4(1.0f,i,i*i, i*i*i);
        for (int j = 0; j < 4; ++j) {
            glm::vec3 row = result[j];
            value += u[j] * row;
        }
        positions.push_back(value);
    }
    return positions;
}


void renderBasicSplineStudy(GLFWwindow* window) {
    //Need tooling to change the interpolation strategy.
    //This tooling can engage certain "modes" required for the interpolation strategy e.g. points and tangents for bezier strategy.
    ShaderProgram program("/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/Polydeukes/Polydeukes/shaders/vertexshader.glsl", "/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/Polydeukes/Polydeukes/shaders/fragmentshader.glsl");
    program.init();
    Camera camera(glm::vec3(0.0f,10.f,35.f), glm::vec3(0.0f,0.0f,0.0f));
    Scene theScene{};
    Renderer renderer(&theScene,&program);
    
    Plane plane(camera.getDirection(), glm::vec3(0.f,0.f,0.f));
    std::vector<std::shared_ptr<Shape>> controlPoints{};
    MousePicker picker = MousePicker(&renderer, &camera, &theScene, [&](double mousePosx, double mousePosy) {
        Ray mouseRay = MousePicker::computeMouseRay(mousePosx, mousePosy);
        glm::vec3 position = vector::rayPlaneIntersection(plane, mouseRay);
        std::shared_ptr<Shape> controlPoint = SphereBuilder::getInstance()->withColour(glm::vec3(1.0f,1.0f,1.0f)).withPosition(position).build();
        renderer.addMesh(controlPoint);
        controlPoints.push_back(controlPoint);
    });
    renderer.addMesh(IconBuilder(&camera)
                     .withOnClickCallback([&](Shape* target) {
                         //compute the interpolating polynomial.
                         std::vector<glm::vec3> positions = computeInterpolatingPolynomial(controlPoints, renderer);
                         std::vector<std::shared_ptr<Shape>> fill{};
                         for (auto pos : positions) {
                             std::shared_ptr<Shape> notch = SquareBuilder().withPosition(pos).withColour(glm::vec3(1.0f,1.0f,1.0f)).build();
                             fill.push_back(notch);
                             renderer.addMesh(notch);
                         }
                         for (auto shape : fill) {
                             shape->setOnClick([fill](Shape* shape) {
                                 for (auto it : fill) {
                                     it->setColour(glm::vec3(0.5f,0.5f,0.5f));
                                 }
                             });
                             shape->setOnMouseUp([fill](Shape* shape) {
                                 for (auto it : fill) {
                                     it->setColour(glm::vec3(1.0f,1.0f,1.0f));
                                 }
                             });
                         }
                         auto lineFill = std::make_shared<std::vector<std::shared_ptr<Shape>>>(fill);

                         for (auto point : controlPoints) {
                             point->setOnMouseDrag([lineFill, &renderer, &controlPoints](Shape* targetShape) {
                                 std::vector<glm::vec3> positions = computeInterpolatingPolynomial(controlPoints, renderer);
                                 for (int i = 0; i < positions.size(); ++i) {
                                     lineFill->at(i)->setModelingTransform(glm::translate(glm::mat4(1.0f), positions[i]));
                                 }
                             });
                         }
                         target->setColour(glm::vec3(0.741,0.706,0.208));
                     })
                     .withOnMouseUpCallback([](Shape* target) {
                         target->setColour(glm::vec3(0.212,0.329,0.369));
                     })
                     .withColour(glm::vec3(0.212,0.329,0.369)).build());
    renderer.addMesh(IconBuilder(&camera).withColour(glm::vec3(0.212,0.329,0.369)).build());
    picker.enable(window);
    MeshDragger::camera = &camera;
    MeshDragger::renderer = &renderer;
    renderer.buildandrender(window, &camera, &theScene);
}


/*
 TODO: Using MVC to define multiple viewing rectangles. Tinker with glViewport and google around to see examples.
 */
int main(int argc, const char * argv[]) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(Renderer::screen_width, Renderer::screen_height, "Polydeukes", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, Renderer::screen_width, Renderer::screen_height);
    
    renderBasicSplineStudy(window);

    glfwTerminate();
    return 0;
}
