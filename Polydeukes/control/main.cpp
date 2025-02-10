//
//  main.cpp
//  LearnOpenGLProject1
//
//  Created by Lawrence Berardelli on 2023-11-04.
//

#define GLFW_INCLUDE_NONE

#include <iostream>
#include <filesystem>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "sierpinski.h"
#include "texture.h"
#include "cube.h"
#include "../model/ShaderProgram.h"
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
#include "../model/spline.h"
#include "../model/objinterpreter.h"
#include "../model/grid.h"
#include "../model/glyph.h"
#include "../view/screenheight.h"
#include "../model/ttfinterpreter.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <functional>
#include <stack>

/*
 TODO: need an undo button.
 */
class KeyboardController {
private:
    
    static Renderer* renderer;
    static std::weak_ptr<Shape> targetShape;
    static ShapeBuilder* sphereBuilder;
    static ShapeBuilder* cubeBuilder;
    static ShapeBuilder* squareBuilder;
    
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
            Particle particle = Particle(cubeBuilder->build(), .1f);
            particle.addTensor(&Gravity::update);
            renderer->addParticle(particle);
            targetShape = particle.getShape();
        }
        else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
            Particle particle = Particle(squareBuilder->build(), .1f);
            particle.addTensor(&Gravity::update);
            renderer->addParticle(particle);
            targetShape = particle.getShape();
        }
        else if (key == GLFW_KEY_M && action == GLFW_PRESS) {
            Particle particle = Particle(sphereBuilder->build(), .1f);
            particle.addTensor(&Gravity::update);
            renderer->addParticle(particle);
            targetShape = particle.getShape();
        }
        else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
            if (auto tmp = targetShape.lock()) {
                glm::vec3 translationVector(0.0f, -1.0f, 0.0f);
                glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translationVector);
                tmp->translate(translationMatrix);
            }
        }
        else if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
            if (auto tmp = targetShape.lock()) {
                glm::vec3 translationVector(0.0f, 1.0f, 0.0f);
                glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translationVector);
                tmp->translate(translationMatrix);
            }
        }
        else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
            if (auto tmp = targetShape.lock()) {
                glm::vec3 translationVector(-1.0f, 0.0f, 0.0f);
                glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translationVector);
                tmp->translate(translationMatrix);
            }
        }
        else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
            if (auto tmp = targetShape.lock()) {
                glm::vec3 translationVector(1.0f, 0.0f, 0.0f);
                glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translationVector);
                tmp->translate(translationMatrix);
            }
        }
        else if (key == GLFW_KEY_B && action == GLFW_PRESS) {
            if (auto tmp = targetShape.lock()) {
                glm::vec3 translationVector(0.0f, 0.0f, -1.0f);
                glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translationVector);
                tmp->translate(translationMatrix);
            }
        }
        else if (key == GLFW_KEY_F && action == GLFW_PRESS) {
            if (auto tmp = targetShape.lock()) {
                glm::vec3 translationVector(0.0f, 0.0f, 1.0f);
                glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translationVector);
                tmp->translate(translationMatrix);
            }
        }
        else if (key == GLFW_KEY_DELETE && action == GLFW_PRESS) {
            if (auto tmp = targetShape.lock()) {
                renderer->removeShape(tmp);
                targetShape.reset();
            }
        }
        else if (key == GLFW_KEY_C && action == GLFW_PRESS) {
            if (auto tmp = targetShape.lock()) {
                std::shared_ptr<Shape> cpy = tmp->clone();
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
        sphereBuilder = &SphereBuilder::getInstance()->withOnClickCallback([&](std::weak_ptr<Shape> shape){ this->setTargetShape(shape); })
        .withOnHoverCallback([](std::weak_ptr<Shape> shape) { shape.lock()->setColour(glm::vec3(1.0f, 0.53f, 0.3f)); })
        .withOffHoverCallback([](std::weak_ptr<Shape> shape) { shape.lock()->setColour(glm::vec3(1.0f, 1.0f,1.0f)); });
        cubeBuilder = new CubeBuilder();
        cubeBuilder = &cubeBuilder->withColour(glm::vec3(1.0f,1.0f,1.0f))
        .withOnClickCallback([&](std::weak_ptr<Shape> shape){ this->setTargetShape(shape); })
        .withOnHoverCallback([](std::weak_ptr<Shape> shape) { shape.lock()->setColour(glm::vec3(1.0f, 0.53f, 0.3f)); })
        .withOffHoverCallback([](std::weak_ptr<Shape> shape) { shape.lock()->setColour(glm::vec3(1.0f, 1.0f,1.0f)); });
        squareBuilder = new SquareBuilder();
        squareBuilder = &squareBuilder->withColour(glm::vec3(1.0f,1.0f,1.0f))
        .withOnClickCallback([&](std::weak_ptr<Shape> shape){ this->setTargetShape(shape); })
        .withOnHoverCallback([](std::weak_ptr<Shape> shape) { shape.lock()->setColour(glm::vec3(1.0f, 0.53f, 0.3f)); })
        .withOffHoverCallback([](std::weak_ptr<Shape> shape) { shape.lock()->setColour(glm::vec3(1.0f, 1.0f,1.0f)); });
        mousepicker->enable(window);
    }
    
    void setTargetShape(std::weak_ptr<Shape> target) {
        targetShape = target;
    }
    
};

Camera* MeshDragger::camera{};
std::weak_ptr<Shape> MeshDragger::targetShape{};


Arcball* KeyboardController::arcball{};
MeshSpawner* KeyboardController::meshSpawner{};
MousePicker* KeyboardController::mousepicker{};
GLFWwindow* KeyboardController::window{};
Renderer* KeyboardController::renderer{};
std::weak_ptr<Shape> KeyboardController::targetShape{};
ShapeBuilder* KeyboardController::sphereBuilder{};
ShapeBuilder* KeyboardController::cubeBuilder{};
ShapeBuilder* KeyboardController::squareBuilder{};


void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
}

std::string getShaderDirectory() {
    std::string fullPath = __FILE__;
    std::string sourceDir = fullPath.substr(0, fullPath.find_last_of("/"));
    return sourceDir.substr(0, sourceDir.find_last_of("/")) + "/shaders/";
}

std::string getFontDirectory() {
    std::string fullPath = __FILE__;
    std::string sourceDir = fullPath.substr(0, fullPath.find_last_of("/"));
    sourceDir = sourceDir.substr(0, sourceDir.find_last_of("/"));
    return sourceDir.substr(0, sourceDir.find_last_of("/")) + "/fonts";
}

SceneList getFloor() {
    std::shared_ptr<Shape> s1 = SquareBuilder().withColour(glm::vec3(0.9,0.9,0.9)).build();
    std::shared_ptr<Shape> s2 = SquareBuilder().withColour(glm::vec3(0.1,0.1,0.1)).build();
    s1->setModelingTransform(glm::rotate(glm::mat4(1.0f), 3.14159f/2.0f, glm::vec3(1.0f,0.0f,0.0f)));
    s2->setModelingTransform(glm::rotate(glm::mat4(1.0f), 3.14159f/2.0f, glm::vec3(1.0f,0.0f,0.0f)));
    std::shared_ptr<Shape> cur;
    std::vector<std::shared_ptr<Shape>> graph{};
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
    ShaderProgram program(getShaderDirectory() + "vertexshader.glsl", getShaderDirectory() + "fragmentshader.glsl");
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
    secondFloor->setOnClick([&](std::weak_ptr<Shape> shape){
        controller.setTargetShape(shape);
    });
    KeyboardController::arcball = &arcball;
    KeyboardController::meshSpawner = &spawner;
    KeyboardController::mousepicker = &picker;
    KeyboardController::window = window;
    controller.init(&renderer);
    std::shared_ptr<Shape> cube = CubeBuilder().withOnHoverCallback([](std::weak_ptr<Shape> shape) {
        shape.lock()->setColour(glm::vec3(1.0f, 0.53f, 0.3f));
    }).withOffHoverCallback([](std::weak_ptr<Shape> shape) {
        shape.lock()->setColour(glm::vec3(1.0f, 1.0f, 1.0f));
    }).withOnClickCallback([&](std::weak_ptr<Shape> shape){
        controller.setTargetShape(shape);
    }).build();
    std::shared_ptr<Shape> springCoil = CubeBuilder().withOnHoverCallback([](std::weak_ptr<Shape> shape) { shape.lock()->setColour(glm::vec3(0.9803921568627451,0.7764705882352941,0.03137254901960784)); })
        .withOffHoverCallback([](std::weak_ptr<Shape> shape) { shape.lock()->setColour(glm::vec3(1.0f,1.0f,1.0f)); }).build();
    std::shared_ptr<Shape> springCoilCopy = springCoil->clone();
    renderer.addMesh(springCoil); renderer.addMesh(springCoilCopy);
    Spring spring = Spring(0.9f,2, glm::vec3(3.0f,3.0f,0.0), springCoil);
    Spring cielingSpring = Spring(0.9f,1,glm::vec3(10.0f,10.0f,0.0f), springCoilCopy);
    Drag drag = Drag(0.1f,0.1f);
    std::shared_ptr<Shape> sphere = SphereBuilder::getInstance()->build();
    sphere->setModelingTransform(glm::scale(glm::mat4(1.0f), glm::vec3(0.5,0.5,0.5)));
    SceneListBuilder sceneGraphBuilder = SceneListBuilder();
    sceneGraphBuilder.withOnHoverCallback([](std::weak_ptr<Shape> shape) {
        shape.lock()->setColour(glm::vec3(1.0f, 0.53f, 0.3f));
    }).withOffHoverCallback([](std::weak_ptr<Shape> shape) {
        shape.lock()->setColour(glm::vec3(1.0f, 1.0f, 1.0f));
    }).withOnClickCallback([&](std::weak_ptr<Shape> shape){
        controller.setTargetShape(shape);
    });
    std::shared_ptr<Shape> subGraph = sceneGraphBuilder.build();
    std::vector<std::shared_ptr<Shape>> tmp;
    tmp.push_back(cube->clone());
    tmp.push_back(AxiesBuilder().build());
    static_cast<SceneList*>(subGraph.get())->addShapes(std::move(tmp));
    static_cast<SceneList*>(subGraph.get())->setPosition(glm::vec3(2.0f,2.0f,2.0f));
    std::shared_ptr<Shape> otherGraph = sceneGraphBuilder.build();
    std::vector<std::shared_ptr<Shape>> tmp2;
    tmp2.push_back(std::move(sphere));
    tmp2.push_back(AxiesBuilder().build());
    static_cast<SceneList*>(otherGraph.get())->addShapes(std::move(tmp2));
    std::shared_ptr<Shape> springEndSpriteS = sceneGraphBuilder.build();
    std::shared_ptr<SceneList> springEndSprite = std::dynamic_pointer_cast<SceneList>(springEndSpriteS);
    std::vector<std::shared_ptr<Shape>> tmp3;
    tmp3.push_back(std::move(otherGraph));
    tmp3.push_back(std::move(subGraph));
    springEndSprite->addShapes(std::move(tmp3));
    springEndSprite->setOnClick([&](std::weak_ptr<Shape> theShape) {
        MeshDragger::registerMousePositionCallback(window, theShape);
    });
    springEndSprite->setPosition(glm::vec3(10.0f,20.0f,0.0f));
    glm::vec3 lookat = springEndSprite->getPosition() - spring.getFixedPosition();
    std::shared_ptr<Shape> square = SquareBuilder().build();
    square->updateModellingTransform(glm::scale(glm::mat4(1.0f), glm::vec3(6.0f,6.0f,6.0f)));
    std::shared_ptr<Shape> platformGraphS = sceneGraphBuilder.build();
    std::shared_ptr<SceneList> platformGraph = std::dynamic_pointer_cast<SceneList>(platformGraphS);
    std::vector<std::shared_ptr<Shape>> tmp4;
    tmp4.push_back(std::move(square));
    tmp4.push_back(AxiesBuilder().build());
    platformGraph->addShapes(std::move(tmp4));
    platformGraph->updateModellingTransform(vector::buildChangeOfBasisMatrix(lookat));
    platformGraph->updateModellingTransform(glm::translate(glm::mat4(1.0f), glm::vec3(3.0f,3.0f,0.0f)));
    std::shared_ptr<Shape> cielingS = sceneGraphBuilder.build();
    std::shared_ptr<SceneList> cieling = std::dynamic_pointer_cast<SceneList>(cielingS);
    std::vector<std::shared_ptr<Shape>> tmp5;
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
        gridProgram(getShaderDirectory() + "lightsourcevs.glsl", getShaderDirectory() + "gridfs.glsl");
    gridProgram.init();
    
    renderer.addParticle(Particle(secondFloor, 0),&gridProgram);
    renderer.addParticle(Particle(northWall, 0),&gridProgram);
    renderer.addParticle(Particle(eastWall, 0),&gridProgram);
    renderer.addParticle(Particle(westWall, 0),&gridProgram);
    
    Particle pellet(SphereBuilder::getInstance()->withOnClickCallback([&](std::weak_ptr<Shape> targetShape) {
        MeshDragger::registerMousePositionCallback(window, targetShape);
    }).build(), 0.10);
    pellet.addTensor(&Gravity::update);
    pellet.addTensor(std::bind(&Drag::update, &drag, std::placeholders::_1, std::placeholders::_2));
    renderer.addParticle(pellet);
    renderer.buildandrender(window, &camera, &theScene);
}

void renderMotionCaptureScene(GLFWwindow* window) {
    ShaderProgram program(getShaderDirectory() + "vertexshader.glsl", getShaderDirectory() + "fragmentshader.glsl");
    program.init();
    Camera camera(glm::vec3(0.,150.f,150.f), glm::vec3(0.f,10.f,0.f));
    Scene theScene{};
    Renderer renderer(&theScene,&program);
    Arcball arcball = Arcball(&camera);
    MousePicker picker = MousePicker(&renderer, &camera, &theScene, [&](double mosPosx, double mosPosy) {
        arcball.registerRotationCallback(window, mosPosx, mosPosy);
    });
    picker.enable(window);
    std::string bvhFile = "/Users/lawrenceberardelli/Documents/bvh_sample_files/thief.bvh";
    std::shared_ptr<SceneGraph> graph(new SceneGraph(bvhFile));
    renderer.addMesh(std::dynamic_pointer_cast<Shape>(graph), &program);
    renderer.buildandrender(window, &camera, &theScene);
}

//TODO: Make a switch to go between 2d and 3d modes
std::vector<glm::vec3> computeFillPositions(glm::mat4 constraintMatrix, std::vector<glm::vec3>& controlPoints) {
    glm::mat4 blendingMatrix = glm::inverse(constraintMatrix);
    glm::mat4x3 result;
    for (int i = 0; i < 4; ++i) {
        glm::vec4 row = blendingMatrix[i];
        glm::vec3 accum = glm::vec3(0.0f,0.0f,0.0f);
        for (int j = 0; j < 4; ++j) {
            accum += row[j] * controlPoints[j];
        }
        result[i] = accum;
    }
    std::vector<glm::vec3> positions{};
    float inc = 0.f;
    for (int i = 0; i < GRANULARITY; i += 1) {
        glm::vec3 value = glm::vec3(0.0f,0.0f,0.0f);
        glm::vec4 u = glm::vec4(1.0f,inc,inc*inc, inc*inc*inc);
        for (int j = 0; j < 4; ++j) {
            glm::vec3 row = result[j];
            value += u[j] * row;
        }
        inc+=1.f/(float)GRANULARITY;
        positions.push_back(value);
    }
    return positions;
}

std::vector<glm::vec3> computeInterpolatingPolynomial(std::vector<glm::vec3>& controlPoints) {
    glm::mat4 constraintMatrix = glm::mat4(
            1.0f, 0.0f,   0.0f,     0.0f,
            1.0f, 1.0f/3.0f, 1.0f/9.0f, 1.0f/27.0f,
            1.0f, 2.0f/3.0f, 4.0f/9.0f, 8.0f/27.0f,
            1.0f, 1.0f,   1.0f,    1.0f
        );
    return computeFillPositions(constraintMatrix, controlPoints);
}

std::vector<glm::vec3> computeInterpolatingPolynomial(std::vector<std::shared_ptr<Shape>>& controlPoints) {
    std::vector<glm::vec3> controlPointPositions{};
    for (auto controlPoint : controlPoints) {
        controlPointPositions.push_back(controlPoint->getPosition());
    }
    return computeInterpolatingPolynomial(controlPointPositions);
}

std::vector<glm::vec3> computeBezierCurve(std::vector<std::shared_ptr<Shape>>& controlPoints) {
    std::array<std::shared_ptr<Shape>, 4> currentCurve;
    std::vector<glm::vec3> positions{};
    for (int i = 0; i < controlPoints.size()-3; i+=3) {
        controlPoints[i+1]->setColour(glm::vec3(1.0f,0.0f,0.0f));
        controlPoints[i+2]->setColour(glm::vec3(1.0f,0.f,0.f));
        currentCurve[0] = controlPoints[i];
        currentCurve[1] = controlPoints[i+1];
        currentCurve[2] = controlPoints[i+2];
        currentCurve[3] = controlPoints[i+3];
        float parameterValue = 0.f;
        for (int j = 0; j < GRANULARITY; ++j) {
            glm::vec3 firstInterpolatedValue = currentCurve[1]->getPosition()*(parameterValue) + currentCurve[0]->getPosition() * (1.f-parameterValue);
            glm::vec3 secondInterpolatedValue = currentCurve[2]->getPosition()*(parameterValue) + currentCurve[1]->getPosition() * (1.f-parameterValue);
            glm::vec3 thirdInterpolatedValue = currentCurve[3]->getPosition()*(parameterValue) + currentCurve[2]->getPosition() * (1.f-parameterValue);
            glm::vec3 secondFirstIV = secondInterpolatedValue*(parameterValue) + firstInterpolatedValue*(1.f-parameterValue);
            glm::vec3 secondsecondIV = thirdInterpolatedValue*(parameterValue) + secondInterpolatedValue*(1.f-parameterValue);
            glm::vec3 thirdFirstIV = secondsecondIV * (parameterValue) + secondFirstIV * (1.f-parameterValue);
            positions.push_back(thirdFirstIV);
            parameterValue += (float)1.f/(float)GRANULARITY;
        }
    }
    return positions;
}

struct HermiteControlPoint {
    std::shared_ptr<Shape> locationSprite;
    std::shared_ptr<Shape> rateOfChangeSprite;
    glm::vec3 rateOfChange;
    
    HermiteControlPoint(std::shared_ptr<Shape> locationSprite, std::shared_ptr<Shape> geo, glm::vec3 rateOfChange) : locationSprite(locationSprite), rateOfChangeSprite(geo), rateOfChange(rateOfChange) {}
};

std::vector<glm::vec3> computeHermiteSpline(std::vector<HermiteControlPoint>& controlPoints) {
    glm::mat4 constrainMatrix = glm::mat4(1.f,0.f,0.f,0.f,0.f,1.f,0.f,0.f,1.f,1.f,1.f,1.f,0.f,1.f,2.f,3.f);
    std::vector<glm::vec3> rawPoints{};
    unsigned long nSegments = controlPoints.size() - 1;
    std::vector<glm::vec3> fillPoints{};
    for (int i = 0; i < nSegments; ++i) {
        rawPoints.push_back(controlPoints[i].locationSprite->getPosition());
        rawPoints.push_back(controlPoints[i].rateOfChange);
        rawPoints.push_back(controlPoints[i+1].locationSprite->getPosition());
        rawPoints.push_back(controlPoints[i+1].rateOfChange);
        std::vector<glm::vec3> fills = computeFillPositions(constrainMatrix, rawPoints);
        for (auto fill : fills) {
            fillPoints.push_back(fill);
        }
        rawPoints.clear();
    }
    return fillPoints;
}

struct LocationIndexPairs {
    std::vector<glm::vec3> locations{};
    std::vector<int> indices{};
};

LocationIndexPairs updateHermiteSpline(std::vector<HermiteControlPoint>& controlPoints, std::shared_ptr<Shape> targetShape) {
    std::vector<HermiteControlPoint> localControlPoints{};
    std::vector<int> indicesToUpdate{};
    for (int i = 0; i < controlPoints.size(); ++i) {
        if (i == 0) {
            if (controlPoints[i].locationSprite.get() == targetShape.get() || controlPoints[i].rateOfChangeSprite.get() == targetShape.get()) {
                localControlPoints.push_back(controlPoints[i]);
                localControlPoints.push_back(controlPoints[i+1]);
                for (int i = 0; i < GRANULARITY; ++i) {
                    indicesToUpdate.push_back(i);
                }
            }
        }
        else if (i == controlPoints.size()-1) {
            if (controlPoints[i].locationSprite.get() == targetShape.get() || controlPoints[i].rateOfChangeSprite.get() == targetShape.get()) {
                localControlPoints.push_back(controlPoints[i-1]);
                localControlPoints.push_back(controlPoints[i]);
                for (int j = (i-1) * GRANULARITY; j < (i-1) * GRANULARITY + GRANULARITY; ++j) {
                    indicesToUpdate.push_back(j);
                }
            }
        }
        else {
            if (controlPoints[i].locationSprite.get() == targetShape.get() || controlPoints[i].rateOfChangeSprite.get() == targetShape.get()) {
                localControlPoints.push_back(controlPoints[i-1]);
                localControlPoints.push_back(controlPoints[i]);
                localControlPoints.push_back(controlPoints[i+1]);
                for (int j = (i-1) * GRANULARITY; j < i * GRANULARITY + GRANULARITY; ++j) {
                    indicesToUpdate.push_back(j);
                }
            }
        }
    }
    std::vector<glm::vec3> locations = computeHermiteSpline(localControlPoints);
    LocationIndexPairs pairs{};
    pairs.indices = indicesToUpdate;
    pairs.locations = locations;
    return pairs;
}

void renderBasicSplineStudy(GLFWwindow* window) {
    ShaderProgram program(getShaderDirectory() + "vertexshader.glsl", getShaderDirectory() + "fragmentshader.glsl");
    program.init();
    Camera camera(glm::vec3(0.0f,10.f,35.f), glm::vec3(0.0f,0.0f,0.0f));
    Scene theScene{};
    Renderer renderer(&theScene,&program);
    Arcball arcball(&camera);
    std::vector<std::shared_ptr<Shape>> controlPoints{};
    std::vector<HermiteControlPoint> hermiteControlPoints{};
    MousePicker picker = MousePicker(&renderer, &camera, &theScene, [&](double mousePosx, double mousePosy) {
        Ray mouseRay = MousePicker::computeMouseRay(mousePosx, mousePosy);
        Plane plane(camera.getDirection(), glm::vec3(0.f,0.f,0.f));
        glm::vec3 position = vector::rayPlaneIntersection(plane, mouseRay);
        std::shared_ptr<Shape> controlPoint = SphereBuilder::getInstance()->withColour(glm::vec3(1.0f,1.0f,1.0f)).build();
        controlPoint->setModelingTransform(glm::scale(glm::mat4(1.0f), glm::vec3(.5f,.5f,.5f)));
        controlPoint->updateModellingTransform(glm::translate(glm::mat4(1.0f), position));
        controlPoints.push_back(controlPoint);
        controlPoint->setOnClick([&](std::weak_ptr<Shape> targetShape) {
            MeshDragger::registerMousePositionCallback(window, targetShape);
        });
        controlPoint->setOnRightClick([&](std::weak_ptr<Shape> targetShape){
            std::shared_ptr<Shape> tmp = CubeBuilder()
                .withPosition(targetShape.lock()->getPosition())
                .withOnClickCallback([&window, targetShape](std::weak_ptr<Shape> theShape) {
                    LineDrawer::registerMousePositionCallback(window, targetShape.lock()->getPosition(), theShape);
                })
                .build();
            renderer.addMesh(tmp);
            LineDrawer::lineData.startPosition = targetShape.lock()->getPosition();
            LineDrawer::registerMousePositionCallback(window, targetShape, tmp);
            targetShape.lock()->setOnClick([&window, tmp](std::weak_ptr<Shape> targetShape){
                std::vector<std::weak_ptr<Shape>> passengers{};
                passengers.push_back(tmp);
                MeshDragger::registerMousePositionCallback(window, targetShape, passengers);
            });
        });
        controlPoint->setOnRightClickUp([&](std::weak_ptr<Shape> theShape) {
            LineDrawer::LineData lineData = LineDrawer::getMostRecentLineData();
            hermiteControlPoints.push_back(HermiteControlPoint(theShape.lock(), lineData.geometry.lock(), lineData.endPosition-lineData.startPosition));
        });
        renderer.addMesh(controlPoint);
    }, [&arcball, window](double x, double y) {
        arcball.registerRotationCallback(window, x, y);
    });
    renderer.addMesh(IconBuilder(&camera)
                     .withOnClickCallback([&](std::weak_ptr<Shape> target) {
                         //compute the interpolating polynomial.
                         std::vector<glm::vec3> positions = computeInterpolatingPolynomial(controlPoints);
                         std::vector<std::shared_ptr<Shape>> fill{};
                         for (auto pos : positions) {
                             std::shared_ptr<Shape> notch = SquareBuilder().withPosition(pos).withColour(glm::vec3(1.0f,1.0f,1.0f)).build();
                             fill.push_back(notch);
                             renderer.addMesh(notch);
                         }
                         for (auto shape : fill) {
                             shape->setOnClick([fill](std::weak_ptr<Shape> shape) {
                                 for (auto it : fill) {
                                     it->setColour(glm::vec3(0.5f,0.5f,0.5f));
                                 }
                             });
                             shape->setOnMouseUp([fill](std::weak_ptr<Shape> shape) {
                                 for (auto it : fill) {
                                     it->setColour(glm::vec3(1.0f,1.0f,1.0f));
                                 }
                             });
                         }
                         auto lineFill = std::make_shared<std::vector<std::shared_ptr<Shape>>>(fill);

                         for (auto point : controlPoints) {
                             point->setOnMouseDrag([lineFill, &renderer, &controlPoints](std::weak_ptr<Shape> targetShape) {
                                 std::vector<glm::vec3> positions = computeInterpolatingPolynomial(controlPoints);
                                 for (int i = 0; i < positions.size(); ++i) {
                                     lineFill->at(i)->setModelingTransform(glm::translate(glm::mat4(1.0f), positions[i]));
                                 }
                             });
                         }
                         target.lock()->setColour(glm::vec3(0.741,0.706,0.208));
                     })
                     .withOnMouseUpCallback([](std::weak_ptr<Shape> target) {
                         target.lock()->setColour(glm::vec3(0.212,0.329,0.369));
                     })
                     .withColour(glm::vec3(0.212,0.329,0.369)).build());
    renderer.addMesh(IconBuilder(&camera)
                     .withOnClickCallback([&](std::weak_ptr<Shape> target) {
                         std::vector<glm::vec3> positions = computeInterpolatingPolynomial(controlPoints);
                         std::vector<std::shared_ptr<Shape>> fill{};
                         for (auto pos : positions) {
                             std::shared_ptr<Shape> notch = SquareBuilder().withPosition(pos).withColour(glm::vec3(1.0f,1.0f,1.0f)).build();
                             notch->updateModellingTransform(glm::scale(glm::mat4(1.0f), glm::vec3(.1f,.1f,.1f)));
                             fill.push_back(notch);
                             renderer.addMesh(notch);
                         }
                     }).withColour(glm::vec3(0.212,0.329,0.369)).build());
    renderer.addMesh(IconBuilder(&camera)
                     .withOnClickCallback([&](std::weak_ptr<Shape> the_icon) {
                         std::vector<glm::vec3> positions = computeHermiteSpline(hermiteControlPoints);
                         std::vector<std::shared_ptr<Shape>> fill{};
                         for (auto pos : positions) {
                             std::shared_ptr<Shape> notch = SquareBuilder().withColour(glm::vec3(1.0f,1.0f,1.0f)).build();
                             notch->setModelingTransform(glm::scale(glm::mat4(1.0f), glm::vec3(.25f,.25f,.25f)));
                             notch->updateModellingTransform(glm::translate(glm::mat4(1.0f), pos));
                             fill.push_back(notch);
                             renderer.addMesh(notch);
                         }
                         auto lineFill = std::make_shared<std::vector<std::shared_ptr<Shape>>>(fill);
                         for (int i = 0; i < hermiteControlPoints.size(); i+=1) {
                             auto point = hermiteControlPoints[i];
                             point.rateOfChangeSprite->setOnMouseDrag([lineFill, &renderer, i, &hermiteControlPoints](std::weak_ptr<Shape> targetShape) {
                                 hermiteControlPoints[i].rateOfChange = LineDrawer::lineData.endPosition - LineDrawer::lineData.startPosition;
                                 LocationIndexPairs pairs = updateHermiteSpline(hermiteControlPoints, targetShape.lock());
                                 int j = 0;
                                 for (auto i : pairs.indices) {
                                     lineFill->at(i)->setModelingTransform(glm::scale(glm::mat4(1.0f), glm::vec3(.25f,.25f,.25f)));
                                     lineFill->at(i)->updateModellingTransform(glm::translate(glm::mat4(1.0f), pairs.locations[j]));
                                     ++j;
                                 }
                             });
                             point.locationSprite->setOnMouseDrag([lineFill, &renderer, i, &hermiteControlPoints](std::weak_ptr<Shape> targetShape) {
                                 LocationIndexPairs pairs = updateHermiteSpline(hermiteControlPoints, targetShape.lock());
                                 int j = 0;
                                 for (auto i : pairs.indices) {
                                     lineFill->at(i)->setModelingTransform(glm::scale(glm::mat4(1.0f), glm::vec3(.25f,.25f,.25f)));
                                     lineFill->at(i)->updateModellingTransform(glm::translate(glm::mat4(1.0f), pairs.locations[j]));
                                     ++j;
                                 }
                             });
                         }
                    }).withColour(glm::vec3(0.212,0.329,0.369)).build());
    renderer.addMesh(IconBuilder(&camera).withOnClickCallback([&](std::weak_ptr<Shape> theIcon){
        std::vector<glm::vec3> positions = computeBezierCurve(controlPoints);
        std::vector<std::shared_ptr<Shape>> lineFill{};
        for (auto pos : positions) {
            std::shared_ptr<Shape> fillShape = SquareBuilder().withColour(glm::vec3(1.0f,1.0f,1.0f)).build();
            fillShape->setModelingTransform(glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, .25f, .25f)));
            fillShape->updateModellingTransform(glm::translate(glm::mat4(1.0f), pos));
            lineFill.push_back(fillShape);
            renderer.addMesh(fillShape);
        }
        std::shared_ptr<std::vector<std::shared_ptr<Shape>>> spLineFill = std::make_shared<std::vector<std::shared_ptr<Shape>>>(lineFill);
        for (auto point : controlPoints) {
            point->setOnMouseDrag([spLineFill, &controlPoints](std::weak_ptr<Shape> theShape){
                std::vector<glm::vec3> positions = computeBezierCurve(controlPoints);
                for (int i = 0; i < positions.size(); ++i) {
                    spLineFill->at(i)->setModelingTransform(glm::scale(glm::mat4(1.0f), glm::vec3(.25f,.25f,.25f)));
                    spLineFill->at(i)->updateModellingTransform(glm::translate(glm::mat4(1.0f), positions[i]));
                }
            });
        }
    }).withColour(glm::vec3(0.212,0.329,.369)).build());
    bool bHidden = false;
    renderer.addMesh(IconBuilder(&camera).withOnClickCallback([&](std::weak_ptr<Shape> theIcon) {
        if (bHidden) {
            for (auto controlPoint : hermiteControlPoints) {
                renderer.addMesh(controlPoint.locationSprite);
                renderer.addMesh(controlPoint.rateOfChangeSprite);
            }
            for (auto controlPoint : controlPoints) {
                renderer.addMesh(controlPoint);
            }
        }
        else {
            for (auto controlPoint : hermiteControlPoints) {
                renderer.removeShape(controlPoint.locationSprite);
                renderer.removeShape(controlPoint.rateOfChangeSprite);
            }
            for (auto point : controlPoints) {
                renderer.removeShape(point);
            }
        }
        bHidden = !bHidden;
    }).build());
    picker.enable(window);
    MeshDragger::camera = &camera;
    LineDrawer::camera = &camera;
    renderer.buildandrender(window, &camera, &theScene);
}

// Evaluate a cubic Bezier component at a given parameter u
float bezierComponent(float P0, float P1, float P2, float P3, float u) {
    float oneMinusU = 1.0f - u;
    return oneMinusU * oneMinusU * oneMinusU * P0 +
           3 * oneMinusU * oneMinusU * u * P1 +
           3 * oneMinusU * u * u * P2 +
           u * u * u * P3;
}

// Solve cubic equation using a robust numerical approach
std::vector<float> solveCubic(float a3, float a2, float a1, float a0) {
    std::vector<float> roots;

    // If the leading coefficient is zero, reduce to a quadratic equation
    if (std::abs(a3) < 1e-6) {
        if (std::abs(a2) < 1e-6) {
            if (std::abs(a1) < 1e-6) {
                return roots; // No solution
            }
            // Linear equation: a1 * u + a0 = 0
            roots.push_back(-a0 / a1);
        } else {
            // Quadratic equation: a2 * u^2 + a1 * u + a0 = 0
            float discriminant = a1 * a1 - 4 * a2 * a0;
            if (discriminant >= 0) {
                roots.push_back((-a1 + std::sqrt(discriminant)) / (2 * a2));
                roots.push_back((-a1 - std::sqrt(discriminant)) / (2 * a2));
            }
        }
    } else {
        // Use Cardano's method for cubic roots (robust numerical implementation omitted here)
        // Placeholder for a full cubic solver like Eigen or specialized libraries
        const int maxIterations = 100;
        const float tolerance = 1e-6;

        for (float guess = 0.0f; guess <= 1.0f; guess += 0.1f) {
            float u = guess;
            for (int iter = 0; iter < maxIterations; ++iter) {
                float f = a3 * u * u * u + a2 * u * u + a1 * u + a0;
                float df = 3 * a3 * u * u + 2 * a2 * u + a1;

                if (std::abs(df) < tolerance) break; // Avoid division by zero
                float nextU = u - f / df;
                if (std::abs(nextU - u) < tolerance) {
                    u = nextU;
                    break;
                }
                u = nextU;
            }

            if (u >= 0.0f && u <= 1.0f) {
                roots.push_back(u);
            }
        }
    }

    // Deduplicate roots within numerical tolerance
    std::sort(roots.begin(), roots.end());
    roots.erase(std::unique(roots.begin(), roots.end(), [&](float a, float b) {
        return std::abs(a - b) < 1e-6;
    }), roots.end());

    return roots;
}

// Find ray-curve intersections
std::vector<std::pair<float, float>> findIntersections(std::vector<glm::vec3> P, float yRay) {
    // Compute cubic coefficients for y(u) - yRay = 0
    float a3 = P[3].y - 3 * P[2].y + 3 * P[1].y - P[0].y;
    float a2 = 3 * (P[2].y - 2 * P[1].y + P[0].y);
    float a1 = 3 * (P[1].y - P[0].y);
    float a0 = P[0].y - yRay;

    // Solve for u
    std::vector<float> uRoots = solveCubic(a3, a2, a1, a0);

    // Compute (t, u) pairs
    std::vector<std::pair<float, float>> intersections;
    for (float u : uRoots) {
        if (u >= 0.0f && u <= 1.0f) {
            float x = bezierComponent(P[0].x, P[1].x, P[2].x, P[3].x, u);
            intersections.emplace_back(x, u);
        }
    }

    // Deduplicate intersections by x-coordinate
    const float tolerance = 1e-6;
    std::sort(intersections.begin(), intersections.end(), [](const auto& a, const auto& b) {
        return a.first < b.first;
    });
    intersections.erase(std::unique(intersections.begin(), intersections.end(), [&](const auto& a, const auto& b) {
        return std::abs(a.first - b.first) < tolerance;
    }), intersections.end());

    return intersections;
}

glm::vec4 clipToScreenSpace(glm::vec4 clipSpaceCoord, float screenWidth, float screenHeight) {
    glm::vec4 screenSpaceCoord;
    clipSpaceCoord = clipSpaceCoord / clipSpaceCoord.w;
    screenSpaceCoord.x = (clipSpaceCoord.x + 1.0f) * 0.5f * screenWidth;
    screenSpaceCoord.y = (1.0f - clipSpaceCoord.y) * 0.5f * screenHeight;
    screenSpaceCoord.z = 0.f;
    screenSpaceCoord.w = clipSpaceCoord.w;
    return screenSpaceCoord;
}

glm::vec4 screenToNDC(int x, int y, int screen_width, int screen_height) {
    float normalizedX = static_cast<float>(x) / screen_width;
    float normalizedY = static_cast<float>(y) / screen_height;
    float ndcX = 2.0f * normalizedX - 1.0f;
    float ndcY = 1.0f - 2.0f * normalizedY;

    return glm::vec4(ndcX, ndcY, 0.f, 1.0f);
}

struct SplineShapeRelation {
    std::vector<std::shared_ptr<Shape>> splines{};
    std::vector<std::shared_ptr<Shape>> controlPoints{};
    
    SplineShapeRelation(std::vector<std::shared_ptr<Shape>> splines, std::vector<std::shared_ptr<Shape>> controlPoints) : splines(splines), controlPoints(controlPoints){}
    SplineShapeRelation(){}
};

void bptInterpreter(GLFWwindow* window, std::vector<std::shared_ptr<Shape>>& controlPoints, SplineShapeRelation& splineSurfaceContainer, Renderer& renderer) {
    //read in bpt file
    ShaderProgram splineSurfaceProgram;
    splineSurfaceProgram.createShaderProgram(getShaderDirectory() + "passthroughvs.glsl", getShaderDirectory() + "splinesurfacetcs.glsl", getShaderDirectory() + "beziersurfacetes.glsl", getShaderDirectory() + "fragmentshader.glsl",
         getShaderDirectory() + "passthroughgs.glsl");
    ShaderProgram splineSurfaceNormalProgram;
    splineSurfaceNormalProgram.createShaderProgram(getShaderDirectory() + "passthroughvs.glsl", getShaderDirectory() + "splinesurfacetcs.glsl", getShaderDirectory() + "beziersurfacetes.glsl", getShaderDirectory() + "lightsourceshader.glsl",
         getShaderDirectory() + "addnormalgs.glsl");
    std::string line;
    std::ifstream myfile("/Users/lawrenceberardelli/Downloads/utah_teapot.bpt");
    int nSurfaces = 0;
    if (myfile.is_open())
    {
        std::getline(myfile,line);
        nSurfaces = std::stoi(line);
        for (int i = 0; i < nSurfaces; ++i) {
            //skip the first line
            std::getline(myfile,line);
            for (int j = 0; j < 16; ++j) {
                std::getline(myfile, line);
                std::istringstream iss(line);
                std::vector<float> position{};
                std::string coordinate{};
                while (iss >> coordinate) {
                    position.push_back(std::stof(coordinate));
                }
                std::shared_ptr<Shape> controlPoint = CubeBuilder().withColour(glm::vec3(1.0f,1.0f,1.0f)).build();
                controlPoint->setModelingTransform(glm::scale(glm::mat4(1.0f), glm::vec3(.1f,.1f,.1f)));
                controlPoint->updateModellingTransform(glm::translate(glm::mat4(1.0f), glm::vec3(position[0], position[1], position[2])));
                controlPoint->setOnClick([&](std::weak_ptr<Shape> targetShape) {
                    MeshDragger::registerMousePositionCallback(window, targetShape);
                });
                controlPoints.push_back(controlPoint);
                splineSurfaceContainer.controlPoints.push_back(controlPoint);
            }
            std::vector<glm::vec3> locations{};
            for (int j = i * 16; j < i*16 + 16; ++j) {
                locations.push_back(controlPoints[j]->getPosition());
            }
            std::shared_ptr<Shape> splineSurface = std::shared_ptr<Shape>(new SplineSurface(locations));
            splineSurface->setColour(glm::vec3(1.0f, 1.0f, 1.0f));
            splineSurfaceContainer.splines.push_back(splineSurface);
        }
        myfile.close();
        int tick = 0;
        std::vector<glm::vec3> startPositions{};
        for (auto controlPoint : controlPoints) {
            startPositions.push_back(controlPoint->getPosition());
        }
        int delay = 0;
        bool bGoingUp = true;
        bool bHidden = false;
        auto foldUnfoldAnimation = [&bHidden, bGoingUp, delay, tick, startPositions, &splineSurfaceContainer, &controlPoints]() mutable {
            // linearly interpolate between current position and some plane
            //in chunks of 16 control points linearly interpolate between -x and x
            ++delay;
            if (delay < 100) {
                return;
            }
            float left_surface = -1.f * (float)splineSurfaceContainer.splines.size() / 5.f;
            //need the size of the surface, say 8 for now.
            for (int i = 0; i < splineSurfaceContainer.splines.size(); ++i) {
                for (int j = 16 * i; j < 16 + 16 * i; ++j) {
                    glm::vec3 targetPosition = glm::vec3(left_surface + (i * .4), -2.f + (j % 4), -2.0f + ((j % 16) / 4));
                    glm::vec3 startPosition = startPositions[j];
                    glm::vec3 newPosition = startPosition * (1-((float)tick/500.f)) + targetPosition * ((float)tick/500.f);
                    std::dynamic_pointer_cast<SplineSurface>(splineSurfaceContainer.splines[i])->updateLocation(j % 16, newPosition);
                    if (!bHidden) {
                        controlPoints[j]->setModelingTransform(glm::scale(glm::mat4(1.0f), glm::vec3(.1,.1,.1)));
                        controlPoints[j]->updateModellingTransform(glm::translate(glm::mat4(1.0f), newPosition));
                    }
                }
            }
            if (bGoingUp) {
                if (tick < 100) {
                    ++tick;
                }
                else {
                    bGoingUp = false;
                }
            }
            else {
                if (tick > 0) {
                    --tick;
                }
                else {
                    bGoingUp = true;
                    delay = -100;
                }
            }
        };
        renderer.addPreRenderCustomization(foldUnfoldAnimation);
    }
    else {
        std::cerr << "Error opening file: " << std::strerror(errno) << std::endl;
        return;
    }
    std::shared_ptr<Shape> bundle = std::shared_ptr<SplineSurfaceBundle>(new SplineSurfaceBundle(splineSurfaceContainer.splines));
    renderer.addMesh(bundle, &splineSurfaceProgram);
    renderer.addMesh(bundle->clone(), &splineSurfaceNormalProgram);
    for (auto controlPoint : controlPoints) {
        renderer.addMesh(controlPoint);
        controlPoint->setOnMouseDrag([&](std::weak_ptr<Shape> targetShape) {
            for (int j = 0; j < splineSurfaceContainer.splines.size(); ++j) {
                for (int i = 0; i < 16; ++i) {
                    if (splineSurfaceContainer.controlPoints[j * 16 + i].get() == targetShape.lock().get()) {
                        std::dynamic_pointer_cast<SplineSurface>(splineSurfaceContainer.splines[j])->updateLocation(i, targetShape.lock()->getPosition());
                    }
                }
            }
        });
    }
}

void splineSurfaceInterpolator(SplineShapeRelation& splineSurfaceContainer, std::vector<std::shared_ptr<Shape>>& controlPoints, Renderer& renderer) {
    //the spline surface interpolator
    ShaderProgram splineSurfaceProgram;
    splineSurfaceProgram.createShaderProgram(getShaderDirectory() + "passthroughvs.glsl", getShaderDirectory() + "splinesurfacetcs.glsl", getShaderDirectory() + "beziersurfacetes.glsl", getShaderDirectory() + "fragmentshader.glsl",
         getShaderDirectory() + "passthroughgs.glsl");
    ShaderProgram splineSurfaceNormalProgram;
    splineSurfaceNormalProgram.createShaderProgram(getShaderDirectory() + "passthroughvs.glsl", getShaderDirectory() + "splinesurfacetcs.glsl", getShaderDirectory() + "beziersurfacetes.glsl", getShaderDirectory() + "lightsourceshader.glsl",
         getShaderDirectory() + "addnormalgs.glsl");
    splineSurfaceContainer.controlPoints = controlPoints;
    std::vector<glm::vec3> locations{};
    for (auto shape : controlPoints) {
        locations.push_back(shape->getPosition());
    }
    std::shared_ptr<Shape> splineSurface = std::shared_ptr<Shape>(new SplineSurface(locations));
    splineSurfaceContainer.splines.push_back(splineSurface);
    for (auto controlPoint : controlPoints) {
        controlPoint->setOnMouseDrag([&](std::weak_ptr<Shape> targetShape) {
            for (int j = 0; j < splineSurfaceContainer.splines.size(); ++j) {
                for (int i = 0; i < 16; ++i) {
                    if (splineSurfaceContainer.controlPoints[j * 16 + i].get() == targetShape.lock().get()) {
                        std::dynamic_pointer_cast<SplineSurface>(splineSurfaceContainer.splines[j])->updateLocation(i, targetShape.lock()->getPosition());
                    }
                }
            }
        });
    }
    renderer.addMesh(splineSurface, &splineSurfaceProgram);
    renderer.addMesh(splineSurface, &splineSurfaceNormalProgram);
}

glm::mat4 getMenuWindowingTransform(Camera& camera, int i, float boundingBox[4]) {
    std::vector<glm::vec3> corners = camera.fovThroughOrigin();
    float w1 = (corners[1].x - corners[0].x) / 9.f;
    float h1 = (corners[1].y - corners[0].y) / 3.f;
    glm::vec2 bl = glm::vec2(boundingBox[0], boundingBox[1]);
    glm::vec2 tr = glm::vec2(boundingBox[2], boundingBox[3]);
    float w2 = tr.x - bl.x;
    float h2 = tr.y - bl.y;
    float sx = w1/(w2+1.f); float sy = h1/(h2 + 1.f);
    glm::vec2 middle = (bl + tr) / (float) 2;
    return glm::translate(glm::mat4(1.0f), glm::vec3(corners[0].x + w1/2.f + (i % 9) * w1, corners[1].y - h1/2.f - (i / 9) * h1, 0.1f)) * glm::scale(glm::mat4(1.0f), glm::vec3(sx, sy, 0.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(-1*middle.x, -1*middle.y, 0.f));
}

void fontEngineMenu(ShaderProgram& splineCurveProgram, ShaderProgram& program, ShaderProgram& glyphProgram, Renderer& renderer, Camera& camera, MousePicker& picker, GLFWwindow* window, Arcball& arcball, std::vector<std::shared_ptr<Shape>>& controlPoints, SplineShapeRelation& splineContainer, std::vector<std::vector<std::vector<glm::vec3>>>& bezierPaths, int& nCurveClicks, int& endOfLastPath, std::shared_ptr<Grid>& grid, std::vector<std::shared_ptr<Shape>>& display, std::vector<std::shared_ptr<Shape>>& icons, bool& bHidden, bool& bAlreadyClicked, std::vector<std::shared_ptr<Shape>>& glyphContainer, std::shared_ptr<FontManager> fontManager);


void fontEditor(ShaderProgram& splineCurveProgram, ShaderProgram& program, ShaderProgram& glyphProgram, Renderer& renderer, Camera& camera, MousePicker& picker, GLFWwindow* window, Arcball& arcball, std::vector<std::shared_ptr<Shape>>& controlPoints, SplineShapeRelation& splineContainer, std::vector<std::vector<std::vector<glm::vec3>>>& bezierPaths, int& nCurveClicks, int& endOfLastPath, std::shared_ptr<Grid>& grid, std::vector<std::shared_ptr<Shape>>& display, std::string outFileName, std::vector<std::shared_ptr<Shape>>& icons, bool& bHidden, bool& bAlreadyClicked, std::vector<std::shared_ptr<Shape>>& glyphContainer, std::shared_ptr<FontManager> fontManager)
{
    //these setup functions are transition operations to execute during transitions between states lol.
    renderer.addMesh(grid);
    for (auto s : display) {
        renderer.removeShape(s);
    }
    for (auto icon : icons) {
        renderer.addMesh(icon);
    }
    for (auto g : glyphContainer) {
        renderer.removeShape(g);
    }
    nCurveClicks = 0;
    endOfLastPath = 0;
    bHidden = false;
    bAlreadyClicked = false;
    controlPoints.clear();
    splineContainer.controlPoints.clear();
    splineContainer.splines.clear();
    bezierPaths.clear();
    glyphContainer.clear();
    
    auto clickCallback = [&, window](double mousePosx, double mousePosy) {
        Ray mouseRay = MousePicker::computeMouseRay(mousePosx, mousePosy);
        Plane plane(camera.getDirection(), glm::vec3(0.f,0.f,0.f));
        glm::vec3 position = vector::rayPlaneIntersection(plane, mouseRay);
        std::shared_ptr<Shape> controlPoint = SphereBuilder::getInstance()->withColour(glm::vec3(1.0f,1.0f,1.0f)).build();
        controlPoint->setModelingTransform(glm::scale(glm::mat4(1.0f), glm::vec3(.1f,.1f,.1f)));
        controlPoint->updateModellingTransform(glm::translate(glm::mat4(1.0f), position));
        controlPoints.push_back(controlPoint);
        controlPoint->setOnClick([window](std::weak_ptr<Shape> targetShape) {
            MeshDragger::registerMousePositionCallback(window, targetShape);
        });
        controlPoint->setOnRightClick([&](std::weak_ptr<Shape> targetShape) {
            controlPoints.push_back(targetShape.lock());
        });
        renderer.addMesh(controlPoint);
    };
    auto rightClickCallback = [&arcball, window](double x, double y) {
        arcball.registerRotationCallback(window, x, y);
    };
    picker.setClickCustomization(clickCallback);
    picker.setRightClickCustomization(rightClickCallback);
    icons[0]->setOnClick([&](std::weak_ptr<Shape> theIcon) {
        if (controlPoints.size() % 4 != 0) {
            return;
        }
        std::shared_ptr<Shape> spline = std::shared_ptr<SplineCurve>(new SplineCurve(controlPoints[nCurveClicks * 4]->getPosition(), controlPoints[nCurveClicks*4 + 1]->getPosition(), controlPoints[nCurveClicks * 4 + 2]->getPosition(), controlPoints[nCurveClicks * 4 + 3]->getPosition()));
        splineContainer.splines.push_back(spline);
        for (int i = 0; i < 4; ++i) {
            if (i == 1 || i == 2) {
                controlPoints[nCurveClicks * 4 + i]->setColour(glm::vec3(1.0f,0.0f,0.f));
            }
            splineContainer.controlPoints.push_back(controlPoints[nCurveClicks * 4 + i]);
        }
        ++nCurveClicks;
        for (auto controlPoint : controlPoints) {
            controlPoint->setOnMouseDrag([&](std::weak_ptr<Shape> targetShape) {
                for (int j = 0; j < splineContainer.splines.size(); ++j) {
                    for (int i = 0; i < 4; ++i) {
                        if (splineContainer.controlPoints[j*4 + i].get() == targetShape.lock().get()) {
                            std::dynamic_pointer_cast<SplineCurve>(splineContainer.splines[j])->updateLocation(i, targetShape.lock()->getPosition());
                        }
                    }
                }
            });
        }
        renderer.addMesh(spline, &splineCurveProgram);
    });
    icons[1]->setOnClick([&](std::weak_ptr<Shape> theIcon) {
        if (!bAlreadyClicked) {
            renderer.removeShape(grid);
            bAlreadyClicked = true;
        } else {
            bAlreadyClicked = false;
            renderer.addMesh(grid);
        }
    });
    icons[2]->setOnClick([&](std::weak_ptr<Shape> theIcon) {
        if (bHidden) {
            for (auto controlPoint : controlPoints) {
                renderer.addMesh(controlPoint);
            }
        }
        else {
            for (auto point : controlPoints) {
                renderer.removeShape(point);
            }
        }
        bHidden = !bHidden;
    });
    icons[3]->setOnClick([&](std::weak_ptr<Shape> theIcon) {
        //define a bezier path
        std::vector<std::vector<glm::vec3>> bezierPath{};
        for (int i = endOfLastPath; i < controlPoints.size()-3; i+=4) {
            std::vector<glm::vec3> segment{};
            for (int j = i; j < i + 4; ++j) {
                segment.push_back(controlPoints[j].get()->getPosition());
            }
            bezierPath.push_back(segment);
        }
        endOfLastPath = controlPoints.size();
        bezierPaths.push_back(bezierPath);
    });
    icons[4]->setOnClick([&, outFileName, window, fontManager](std::weak_ptr<Shape> theIcon) {
        //output the bezier paths
        if (!bezierPaths.empty()) {
            std::string dir = getFontDirectory();
            std::ofstream of(dir + "/" + outFileName, std::ios::out);
            if (of.is_open()) {
                of << "paths:\n";
                for (auto path : bezierPaths) {
                    of << "\tpath:\n";
                    for (auto segment : path) {
                        of << "\t\tsegment:\n";
                        for (auto position : segment) {
                            of << "\t\t\t" << position.x << " " << position.y << " " << position.z << std::endl;
                        }
                    }
                }
            }
            else {
                std::cout << "Failed to open file!" << std::endl;
            }
            of.close();
        }
        auto glyph = FontLoader::reloadGlyph(getFontDirectory(), outFileName, fontManager);
        glyph->setModelingTransform(getMenuWindowingTransform(camera, std::stoi(outFileName), glyph->boundingBox));
        //go back to main menu.
        fontEngineMenu(splineCurveProgram, program, glyphProgram, renderer, camera, picker, window, arcball, controlPoints, splineContainer, bezierPaths, nCurveClicks, endOfLastPath, grid, display, icons, bHidden, bAlreadyClicked, glyphContainer, fontManager);
    });
}

void fontEngineMenu(ShaderProgram& splineCurveProgram, ShaderProgram& program, ShaderProgram& glyphProgram, Renderer& renderer, Camera& camera, MousePicker& picker, GLFWwindow* window, Arcball& arcball, std::vector<std::shared_ptr<Shape>>& controlPoints, SplineShapeRelation& splineContainer, std::vector<std::vector<std::vector<glm::vec3>>>& bezierPaths, int& nCurveClicks, int& endOfLastPath, std::shared_ptr<Grid>& grid, std::vector<std::shared_ptr<Shape>>& display, std::vector<std::shared_ptr<Shape>>& icons, bool& bHidden, bool& bAlreadyClicked, std::vector<std::shared_ptr<Shape>>& glyphContainer, std::shared_ptr<FontManager> fontManager)
{
    int i = 0;
    for (auto icon : icons) {
        renderer.removeShape(icon);
    }
    for (auto spline : splineContainer.splines) {
        renderer.removeShape(spline);
    }
    for (auto controlPoint : splineContainer.controlPoints) {
        renderer.removeShape(controlPoint);
    }
    for (auto glyph : glyphContainer) {
        renderer.removeShape(glyph);
    }
    controlPoints.clear();
    splineContainer.splines.clear();
    splineContainer.controlPoints.clear();
    renderer.removeShape(grid);
    glyphContainer.clear();
    for (int i = 0; i < 27; ++i) {
        auto glyph = fontManager->get(i);
        if (glyph != nullptr) {
            glyphContainer.push_back(glyph);
            renderer.addMesh(glyph, &glyphProgram);
        } else {
            std::cout << "Glyph not ready " << i << std::endl;
        }
    }
    for (auto square : display) {
        renderer.addMesh(square);
        square->setOnClick([&, window, i, fontManager](std::weak_ptr<Shape> theIcon) {
            fontEditor(splineCurveProgram, program, glyphProgram, renderer, camera, picker, window, arcball, controlPoints, splineContainer, bezierPaths, nCurveClicks, endOfLastPath, grid, display, std::to_string(i), icons, bHidden, bAlreadyClicked, glyphContainer, fontManager);
        });
        ++i;
    }
}

void ttfInterpreter(GLFWwindow* window) {
    ShaderProgram splineCurveProgram;
    splineCurveProgram.createShaderProgram(getShaderDirectory() + "passthroughvs.glsl", getShaderDirectory() + "splinecurvetcs.glsl", getShaderDirectory() + "beziertes.glsl", getShaderDirectory() + "splinefs.glsl");
    ShaderProgram program(getShaderDirectory() + "vertexshader.glsl", getShaderDirectory() + "fragmentshader.glsl");
    ShaderProgram glyphProgram(getShaderDirectory() + "glyphvs.glsl", getShaderDirectory() + "glyphfs.glsl");
    program.init();
    glyphProgram.init();
    Scene theScene{};
    Renderer renderer(&theScene,&program);
    Camera camera(glm::vec3(0.0f,0.f,10.f), glm::vec3(0.0f,0.0f,0.0f));
    std::vector<glm::vec3> corners = camera.fovThroughOrigin();
    TTFont font = interpret();
    auto grid = std::shared_ptr<Grid>(new Grid(corners[0], corners[1], font.unitsPerEm / 10.f));
    for (int j = 3; j < 4; ++j) {
        TTFGlyph glyph = font.glyphs[j];
        glm::vec2 centre = glm::vec2((glyph.boundingBox[2]-glyph.boundingBox[0])/2.f, (glyph.boundingBox[3] - glyph.boundingBox[1])/2.f);
        glm::mat4 emToWorld = glm::scale(glm::mat4(1.0f), glm::vec3((corners[1].x - corners[0].x)/((float)font.unitsPerEm * 1.1f), (corners[1].y - corners[0].y)/((float)font.unitsPerEm * 1.1f), 1.f)) * glm::translate(glm::mat4(1.0f), glm::vec3(-1.f * centre.x, -1.f * centre.y, 0.0f));
        glm::vec2 prevLocation = glm::vec2(0,0);
        for (auto contour : font.glyphs[j].contours) {
            for (int i = 0; i < contour.points.size(); ++i) {
                Point point = contour.points[i];
                glm::vec2 currentLocation = glm::vec2(point.xCoord + prevLocation.x, point.yCoord + prevLocation.y);
                glm::vec4 pt = glm::vec4(currentLocation.x, currentLocation.y, 0.f, 1.0f);
                pt = emToWorld * pt;
                auto controlPoint = SphereBuilder::getInstance()->build();
                controlPoint->setModelingTransform(glm::scale(glm::mat4(1.0f), glm::vec3(0.1f,0.1f,0.1f)));
                controlPoint->updateModellingTransform(glm::translate(glm::mat4(1.0f), glm::vec3(pt.x, pt.y, pt.z)));
                if (point.onCurve) {
                    controlPoint->setColour(glm::vec3(1.0f, 0.f,0.f));
                }
                renderer.addMesh(controlPoint);
                prevLocation = currentLocation;
            }
        }
        break;
    }
    renderer.addMesh(grid);
    renderer.buildandrender(window, &camera, &theScene);
}

void renderFontEngine(GLFWwindow* window) {
    ShaderProgram splineCurveProgram;
    splineCurveProgram.createShaderProgram(getShaderDirectory() + "passthroughvs.glsl", getShaderDirectory() + "splinecurvetcs.glsl", getShaderDirectory() + "beziertes.glsl", getShaderDirectory() + "splinefs.glsl");
    ShaderProgram program(getShaderDirectory() + "vertexshader.glsl", getShaderDirectory() + "fragmentshader.glsl");
    ShaderProgram glyphProgram(getShaderDirectory() + "glyphvs.glsl", getShaderDirectory() + "glyphfs.glsl");
    program.init();
    glyphProgram.init();
    Camera camera(glm::vec3(0.0f,0.f,10.f), glm::vec3(0.0f,0.0f,0.0f));
    camera.enableFreeCameraMovement(window);
    Scene theScene{};
    Renderer renderer(&theScene,&program);
    Arcball arcball(&camera);
    std::vector<std::shared_ptr<Shape>> controlPoints{};
    MousePicker picker = MousePicker(&renderer, &camera, &theScene, [](double,double){}, [](double,double){});
    SplineShapeRelation relation{};
    std::vector<std::vector<std::vector<glm::vec3>>> bezierPaths{};
    int nCurveClicks = 0; int nBezierPaths = 0;
    std::vector<glm::vec3> corners = camera.fovThroughOrigin();
    auto grid = std::shared_ptr<Grid>(new Grid(corners[0], corners[1], 40));
    //define the dimensions of the 27 squares that make up the 9x3 display.
    float width = corners[1].x - corners[0].x;
    float height = corners[1].y - corners[0].y;
    std::vector<std::shared_ptr<Shape>> menuDisplay{};
    std::array<std::function<void(std::shared_ptr<Glyph>)>, 27> onGlyphReadyCallbacks{};
    std::vector<std::shared_ptr<Shape>> glyphContainer{};
    for (int i = 0; i < 27; ++i) {
        auto square = SquareBuilder().build();
        square->setModelingTransform(glm::scale(glm::mat4(1.0f), glm::vec3(width/9.f,height/3.f,1.f)));
        square->updateModellingTransform(glm::translate(glm::mat4(1.0f), glm::vec3(corners[0].x + width/18.f + (i % 9) * width/9.f, corners[1].y - height/6.f - (i / 9) * (height / 3.f), 0.f)));
        float f = (float)i / 100.f;
        square->setColour(glm::vec3(f, f, f));
        menuDisplay.push_back(square);
        std::function<void(std::shared_ptr<Glyph>)> cb = [&, i](std::shared_ptr<Glyph> glyph) {
            glyph->setModelingTransform(getMenuWindowingTransform(camera, i, glyph->boundingBox));
            glyphContainer.push_back(glyph);
            renderer.addMesh(glyph, &glyphProgram);
        };
        onGlyphReadyCallbacks[i] = cb;
    }
    std::string fontDirectory = getFontDirectory();
    std::shared_ptr<FontManager> fontManager = FontLoader::loadFont(fontDirectory, onGlyphReadyCallbacks);
    std::vector<std::shared_ptr<Shape>> icons{};
    for (int i = 0; i < 5; ++i) {
        icons.push_back(IconBuilder(&camera).build());
    }
    bool bHidden = false; bool bAlreadyClicked = false;
    fontEngineMenu(splineCurveProgram, program, glyphProgram, renderer, camera, picker, window, arcball, controlPoints, relation, bezierPaths, nCurveClicks, nBezierPaths, grid, menuDisplay, icons, bHidden, bAlreadyClicked, glyphContainer, fontManager);
    picker.enable(window);
    MeshDragger::camera = &camera;
    renderer.buildandrender(window, &camera, &theScene);
}

class Chip8InputHandler {
    static const int constexpr key_codes[16] = {
        GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3, GLFW_KEY_4, GLFW_KEY_Q, GLFW_KEY_W, GLFW_KEY_E, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_F, GLFW_KEY_Z, GLFW_KEY_X, GLFW_KEY_C, GLFW_KEY_V, 10
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
        for (int i = 0; i < 5; ++i) {
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


bool Chip8InputHandler::is_key_pressed[16] = {
    false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false
};

void chipEightInterpreter(GLFWwindow* window) {
    ShaderProgram program(getShaderDirectory() + "vertexshader.glsl", getShaderDirectory() + "chip8fragmentshader.glsl");
    program.init();
    Camera camera(glm::vec3(0.0f,0.0f,35.f), glm::vec3(0.0f,0.0f,0.0f));
    Scene theScene{};
    Renderer renderer(&theScene,&program);
    
    //define the 64x32 display
    std::array<std::shared_ptr<Shape>, 2048> display{};
    //hack based on hard coded fov and aspect ratio from the renderer
    glm::vec3 center = glm::vec3(-25.37, 14.04, 0.f);
    for (int i = 0; i < 32; ++i) {
        for (int j = 0; j < 64; ++j) {
            std::shared_ptr<Shape> tmp = SquareBuilder().withColour(glm::vec3(0.f,0.f,0.f)).withOnClickCallback([](std::weak_ptr<Shape> self){
                self.lock()->setColour(glm::vec3(1.0f,1.0f,1.0f));
            }).build();
            tmp->setModelingTransform(glm::scale(glm::mat4(1.0f), glm::vec3(.805415260f,.906092f, 1.f)));
            tmp->updateModellingTransform(glm::translate(glm::mat4(1.0f), center));
            center = glm::vec3(center.x + .805415260f, center.y, center.z);
            renderer.addMesh(tmp);
            display[i*64+j] = tmp;
        }
        center = glm::vec3(-25.37, center.y - .906092f, 0.f);
    }
    
    //define the memory
    std::array<unsigned int, 4096> ram{};
    //pc
    unsigned int programCounter = 0x200;
    //index register
    unsigned int indexRegister{};
    std::stack<unsigned int> stack{};
    unsigned int soundTimer = 255;
    unsigned int delayTimer = 255;
    std::array<unsigned int, 16> registers{};
    
    //glyphs
    const unsigned int digitBitmaps[16][5] = {
        {0xF0, 0x90, 0x90, 0x90, 0xF0}, // 0
        {0x20, 0x60, 0x20, 0x20, 0x70}, // 1
        {0xF0, 0x10, 0xF0, 0x80, 0xF0}, // 2
        {0xF0, 0x10, 0xF0, 0x10, 0xF0}, // 3
        {0x90, 0x90, 0xF0, 0x10, 0x10}, // 4
        {0xF0, 0x80, 0xF0, 0x10, 0xF0}, // 5
        {0xF0, 0x80, 0xF0, 0x90, 0xF0}, // 6
        {0xF0, 0x10, 0x20, 0x40, 0x40}, // 7
        {0xF0, 0x90, 0xF0, 0x90, 0xF0}, // 8
        {0xF0, 0x90, 0xF0, 0x10, 0xF0}, // 9
        {0xF0, 0x90, 0xF0, 0x90, 0x90}, // A
        {0xE0, 0x90, 0xE0, 0x90, 0xE0}, // B
        {0xF0, 0x80, 0x80, 0x80, 0xF0}, // C
        {0xE0, 0x90, 0x90, 0x90, 0xE0}, // D
        {0xF0, 0x80, 0xF0, 0x80, 0xF0}, // E
        {0xF0, 0x80, 0xF0, 0x80, 0x80}  // F
    };
    //load glyphs to ram
    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 5; ++j) {
            ram[i * 5 + j] = digitBitmaps[i][j];
        }
    }
    //load game into ram
    std::ifstream inputFile("/Users/lawrenceberardelli/Downloads/Sierpinski [Sergey Naydenov, 2010].ch8", std::ios::binary);
    if (!inputFile) {
        std::cerr << "Failed to open the file." << std::endl;
        return;
    }
    char buffer[2];
    int i = 0;
    while (inputFile.read(buffer, 2)) {
        ram[0x200 + i] = static_cast<unsigned char>(buffer[0]); // Assign first byte
        ram[0x200 + i + 1] = static_cast<unsigned char>(buffer[1]); // Assign second byte
        std::cout << std::hex << ram[512+i] << " " << ram[513+i] << ", ";
        i += 2;
    }
    Arcball arcball(&camera);
    arcball.enable(window);
    Chip8InputHandler handler(window);
    Chip8Interpreter interpreter(ram, programCounter, indexRegister, stack, soundTimer, delayTimer, registers, &display, &handler);
    renderer.addPreRenderCustomization([&interpreter]() {
        interpreter.fetchDecodeExecute();
    });
    renderer.buildandrender(window, &camera, &theScene);
}

std::vector<int> parseFaceLine(std::string delim, std::string line) {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;
    while (std::getline(ss, token, ' ')) {
        if (delim != "") {
            token = token.substr(0, token.find(delim, 0));
        }
        tokens.push_back(token);
    }
    std::vector<int> face{};
    for (int i = 1; i < tokens.size(); ++i) {
        face.push_back(std::stoi(tokens[i])-1);
    }
    return face;
}

void objFileInterpeter(GLFWwindow* window) {
    std::string objFile = "/Users/lawrenceberardelli/Downloads/hand.obj";
    ShaderProgram program(getShaderDirectory() + "vertexshader.glsl", getShaderDirectory() + "fragmentshader.glsl");
    program.init();
    Camera camera(glm::vec3(0.0f,0.0f,35.f), glm::vec3(0.0f,0.0f,0.0f));
    Arcball arcball(&camera);
    camera.enableFreeCameraMovement(window);
    arcball.enable(window);
    Scene theScene{};
    Renderer renderer(&theScene,&program);
    std::shared_ptr<Shape> shape = objInterpreter::interpretObjFile(objFile);
    renderer.addMesh(shape);
    MousePicker picker = MousePicker(&renderer, &camera, &theScene, [&](double mosPosx, double mosPosy) {
        arcball.registerRotationCallback(window, mosPosx, mosPosy);
    });
    picker.enableRayTrianglePicker(window);
    renderer.buildandrender(window, &camera, &theScene);
}

void riggingModule(GLFWwindow* window) {
    std::string objFile = "/Users/lawrenceberardelli/Downloads/hand.obj";
    ShaderProgram program(getShaderDirectory() + "vertexshader.glsl", getShaderDirectory() + "fragmentshader.glsl");
    program.init();
    Camera camera(glm::vec3(0.0f,0.0f,35.f), glm::vec3(0.0f,0.0f,0.0f));
    Arcball arcball(&camera);
    Scene theScene{};
    MeshDragger::camera = &camera;
    Renderer renderer(&theScene,&program);
    auto lineFill = CubeBuilder().withColour(glm::vec3(.0f,0.0f,0.0f)).withOnClickCallback([&window](auto theShape){
        MeshDragger::registerMousePositionCallback(window, theShape);
    }).build();
    LineDrawer::camera = &camera;
    auto picker = MousePicker(&renderer, &camera, &theScene, [&](double x, double y) { arcball.registerRotationCallback(window, x, y);});
    camera.enableFreeCameraMovement(window);
    picker.enableRayTrianglePicker(window);
    std::shared_ptr<Shape> shape = objInterpreter::interpretObjFile(objFile);
    std::vector<std::shared_ptr<Shape>> bones{};
    shape->setOnClick([&](std::weak_ptr<Shape> theShape, glm::vec3 exactPosition) {
        auto fillcpy = lineFill->clone();
        fillcpy->setModelingTransform(glm::translate(glm::mat4(1.0f), exactPosition));
        renderer.addMesh(fillcpy);
        LineDrawer::registerMousePositionCallback(window, exactPosition, fillcpy);
        bones.push_back(fillcpy);
    });
    shape->setOnMouseUp([&](std::weak_ptr<Shape> theShape) {
        glm::vec3 endPosition = LineDrawer::lineData.endPosition;
        glm::vec3 startPosition = LineDrawer::lineData.startPosition;
        glm::vec3 direction = glm::normalize(endPosition - startPosition);
        glm::vec3 arbitrary = (fabs(direction.x) > 0.1f) ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);
        glm::vec3 u = glm::normalize(glm::cross(direction, arbitrary));
        glm::vec3 v = glm::cross(direction, u);
        struct RayShapePair {
            Ray ray{};
            std::shared_ptr<Shape> shape{};
        };
        for (int i = 0; i <= 3; ++i) {
            glm::vec3 cur = startPosition * (float)(1 - (i)/(3.f)) + endPosition * ((float)(i)/3.f);
            //shoot rays radially about this current position and intersect them with the triangle mesh.
            std::vector<RayShapePair> rays{};
            for (int i = 0; i < 100; ++i) {
                float theta = (2.0f * glm::pi<float>() * i) / 100; // Angle in radians
                glm::vec3 rayDirection = cos(theta) * u + sin(theta) * v;
                Ray ray; ray.origin = cur; ray.direction = rayDirection;
                auto line = CubeBuilder().build();
                line->setModelingTransform(vector::scaleGeometryBetweenTwoPointsTransformation(ray.origin + ray.direction * 1.f, ray.origin));
                renderer.addMesh(line);
                RayShapePair pair; pair.ray = ray; pair.shape = line;
                rays.push_back(pair); // Ray direction from the origin
            }
            std::vector<Triangle> candidates;
            for (auto ray : rays) {
                std::vector<glm::vec3> triangles = theShape.lock()->getPositions();
                bool bAddedRay = false;
                for (int i = 0; i < triangles.size(); i+=3) {
                    Triangle triangle(triangles[i], triangles[i+1], triangles[i+2]);
                    auto result = vector::rayTriangleIntersection(ray.ray, triangle);
                    for (auto res : result) {
                        if (std::find_if(candidates.begin(), candidates.end(), [&triangle](const Triangle& e){
                            return triangle.a == e.a && triangle.b == e.b && triangle.c == e.c;
                        }) == candidates.end()) {
                            candidates.push_back(triangle);
                            if (glm::length(res - ray.ray.origin) > 2.f) {
                                continue;
                            }
                            ray.shape->setModelingTransform(vector::scaleGeometryBetweenTwoPointsTransformation(res, ray.ray.origin));
                            ray.shape->setColour(glm::vec3(0.0f,1.0f,0.0f));
                            bAddedRay = true;
                        }
                    }
                }
                if (!bAddedRay) {
                    renderer.removeShape(ray.shape);
                }
            }
            std::shared_ptr<Shape> meshoon = std::shared_ptr<ArbitraryShape>(new ArbitraryShape(candidates));
            meshoon->setColour(glm::vec3(0.0f,1.0f,0.0f));
            renderer.addMesh(meshoon);
        }
    });
    renderer.addMesh(shape);
    renderer.buildandrender(window, &camera, &theScene);
}

void window_size_callback(GLFWwindow* window, int width, int height) {
    std::cout << "Window resized to " << width << "x" << height << std::endl;
    ScreenHeight::screen_width = width;
    ScreenHeight::screen_height = height;
    glViewport(0, 0, width, height);
}


/*
 TODO: Using MVC to define multiple viewing rectangles. Tinker with glViewport and google around to see examples.
 */
int main(int argc, const char * argv[]) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(ScreenHeight::screen_width, ScreenHeight::screen_height, "Polydeukes", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeCallback(window, window_size_callback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    const GLubyte* version = glGetString(GL_VERSION);
    std::cout << "OpenGL Version: " << version << std::endl;
    GLint maxBlockSize;
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxBlockSize);
    std::cout << "UBO size: " << maxBlockSize << std::endl;
    glViewport(0, 0, ScreenHeight::screen_width, ScreenHeight::screen_height);
    ttfInterpreter(window);

    glfwTerminate();
    return 0;
}
