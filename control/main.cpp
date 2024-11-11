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
#include "../model/spline.h"

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

int GRANULARITY = 100;

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
    ShaderProgram program("/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/Polydeukes/Polydeukes/shaders/vertexshader.glsl", "/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/Polydeukes/Polydeukes/shaders/fragmentshader.glsl");
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

void renderGPUSplineStudy(GLFWwindow* window) {
    ShaderProgram splineCurveProgram;
    splineCurveProgram.createShaderProgram("/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/Polydeukes/Polydeukes/shaders/passthroughvs.glsl", "/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/Polydeukes/Polydeukes/shaders/splinecurvetcs.glsl", "/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/Polydeukes/Polydeukes/shaders/beziertes.glsl", "/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/Polydeukes/Polydeukes/shaders/splinefs.glsl");
    ShaderProgram splineSurfaceProgram;
    splineSurfaceProgram.createShaderProgram("/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/Polydeukes/Polydeukes/shaders/passthroughvs.glsl", "/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/Polydeukes/Polydeukes/shaders/splinesurfacetcs.glsl", "/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/Polydeukes/Polydeukes/shaders/beziersurfacetes.glsl", "/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/Polydeukes/Polydeukes/shaders/fragmentshader.glsl",
         "/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/Polydeukes/Polydeukes/shaders/passthroughgs.glsl");
    ShaderProgram splineSurfaceNormalProgram;
    splineSurfaceNormalProgram.createShaderProgram("/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/Polydeukes/Polydeukes/shaders/passthroughvs.glsl", "/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/Polydeukes/Polydeukes/shaders/splinesurfacetcs.glsl", "/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/Polydeukes/Polydeukes/shaders/beziersurfacetes.glsl", "/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/Polydeukes/Polydeukes/shaders/lightsourceshader.glsl",
         "/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/Polydeukes/Polydeukes/shaders/addnormalgs.glsl");
    ShaderProgram program("/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/Polydeukes/Polydeukes/shaders/vertexshader.glsl", "/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/Polydeukes/Polydeukes/shaders/fragmentshader.glsl");
    program.init();
    Camera camera(glm::vec3(0.0f,0.f,10.f), glm::vec3(0.0f,0.0f,0.0f));
    camera.enableFreeCameraMovement(window);
    Scene theScene{};
    Renderer renderer(&theScene,&program);
    Arcball arcball(&camera);
    std::vector<std::shared_ptr<Shape>> controlPoints{};
    std::vector<HermiteControlPoint> hermiteControlPoints{};
    struct SplineShapeRelation {
        std::vector<std::shared_ptr<Shape>> splines{};
        std::vector<std::shared_ptr<Shape>> controlPoints{};
        
        SplineShapeRelation(std::vector<std::shared_ptr<Shape>> splines, std::vector<std::shared_ptr<Shape>> controlPoints) : splines(splines), controlPoints(controlPoints){}
        SplineShapeRelation(){}
    };
    SplineShapeRelation splineContainer{};
    SplineShapeRelation splineSurfaceContainer{};
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
        renderer.addMesh(controlPoint);
    }, [&arcball, window](double x, double y) {
        arcball.registerRotationCallback(window, x, y);
    });
    int nCurveClicks = 0;
    renderer.addMesh(IconBuilder(&camera).withOnClickCallback([&](std::weak_ptr<Shape> theIcon) {
        if (controlPoints.size() % 4 != 0) {
            return;
        }
        std::shared_ptr<Shape> spline = std::shared_ptr<SplineCurve>(new SplineCurve(controlPoints[nCurveClicks * 4]->getPosition(), controlPoints[nCurveClicks*4 + 1]->getPosition(), controlPoints[nCurveClicks * 4 + 2]->getPosition(), controlPoints[nCurveClicks * 4 + 3]->getPosition()));
        splineContainer.splines.push_back(spline);
        for (int i = 0; i < 4; ++i) {
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
    }).withColour(glm::vec3(0.212,0.329,.369)).build());
    bool bHidden = false;
    renderer.addMesh(IconBuilder(&camera).withOnClickCallback([&](std::weak_ptr<Shape> theIcon) {
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
//        renderer.addMesh(splineSurface, &splineSurfaceNormalProgram);
    }).withColour(glm::vec3(0.212,0.329,.369)).build());
    bool bAlreadyClicked = false;
    renderer.addMesh(IconBuilder(&camera).withOnClickCallback([&](std::weak_ptr<Shape> theIcon) {
        if (bAlreadyClicked) {
            return;
        }
        //read in bpt file
        std::string line;
        std::ifstream myfile("/Users/lawrenceberardelli/Downloads/finger.bpt");
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
                splineSurface->setColour(glm::vec3(float(i)/(float)nSurfaces, 0.0f, 0.0f));
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
            //renderer.addPreRenderCustomization(foldUnfoldAnimation);
        }
        else {
            std::cerr << "Error opening file: " << std::strerror(errno) << std::endl;
            return;
        }
        bAlreadyClicked = true;
        std::shared_ptr<Shape> bundle = std::shared_ptr<SplineSurfaceBundle>(new SplineSurfaceBundle(splineSurfaceContainer.splines));
        renderer.addMesh(bundle, &splineSurfaceProgram);
        //renderer.addMesh(bundle->clone(), &splineSurfaceNormalProgram);
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
    }).withColour(glm::vec3(0.212,0.329,.369)).build());
    renderer.addMesh(IconBuilder(&camera).withOnClickCallback([&](std::weak_ptr<Shape> theIcon) {
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
    }).build());
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
    ShaderProgram program("/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/Polydeukes/Polydeukes/shaders/vertexshader.glsl", "/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/Polydeukes/Polydeukes/shaders/chip8fragmentshader.glsl");
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
    std::ifstream inputFile("/Users/lawrenceberardelli/Downloads/snake.ch8", std::ios::binary);
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


void objFileInterpeter(GLFWwindow* window) {
    std::string objFile = "/Users/lawrenceberardelli/Downloads/cow.obj";
    std::ifstream inputFile(objFile);
    if (!inputFile) {
        std::cerr << "Failed to open the file " << objFile << std::endl;
    }
    std::string line{};
    std::vector<glm::vec3> positions{};
    std::vector<std::array<int, 3>> faces{};
    while (std::getline(inputFile,line)) {
        if (line.at(0) == '#') {
            continue;
        }
        if (line.at(0) == 'v') {
            std::vector<std::string> tokens;
            std::stringstream ss(line);
            std::string token;
            while (std::getline(ss, token, ' ')) {
                tokens.push_back(token);
            }
            glm::vec3 position = glm::vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
            positions.push_back(position);
            continue;
        }
        if (line.at(0) == 'f') {
            std::vector<std::string> tokens;
            std::stringstream ss(line);
            std::string token;
            while (std::getline(ss, token, ' ')) {
                tokens.push_back(token);
            }
            std::array<int, 3> face{std::stoi(tokens[1])-1, std::stoi(tokens[2])-1, std::stoi(tokens[3])-1};
            faces.push_back(face);
            continue;
        }
    }
    class ArbitraryShape : public Shape {
    private:
        std::vector<float> vertices;
        std::vector<int> indices;
        unsigned int VAO, VBO, EBO;
        
        ArbitraryShape(std::vector<float> vertices, std::vector<int> indices, unsigned int VAO, unsigned int VBO, unsigned int EBO) : vertices(vertices), indices(indices), VAO(VAO), VBO(VBO), EBO(EBO) {}
    public:
        ArbitraryShape(std::vector<glm::vec3> positions, std::vector<std::array<int, 3>> faces) {
            for (auto pos : positions) {
                vertices.push_back(pos.x);
                vertices.push_back(pos.y);
                vertices.push_back(pos.z);
                std::cout << pos.x << " " << pos.y << " " << pos.z << std::endl;
            }
            for (auto face : faces) {
                for (int i = 0; i < 3; ++i) {
                    indices.push_back(face[i]);
                    std::cout << face[i] << " ";
                }
                std::cout << std::endl;
            }
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &EBO);
            glGenBuffers(1, &VBO);
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(float) * 3, (void*)0);
            glEnableVertexAttribArray(0);
            glBindVertexArray(0);
        }
        
        void render(ShaderProgram shaderProgram) {
            shaderProgram.setMat4("model", modellingTransform);
            shaderProgram.setVec3("aColour", colour);
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT, 0);
        }
        
        std::shared_ptr<Shape> clone() {
            return std::shared_ptr<ArbitraryShape>(new ArbitraryShape(vertices, indices, VAO, VBO, EBO));
        }
    };
    ShaderProgram program("/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/Polydeukes/Polydeukes/shaders/basicVertexShader.glsl", "/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/Polydeukes/Polydeukes/shaders/fragmentshader.glsl");
    program.init();
    Camera camera(glm::vec3(0.0f,0.0f,35.f), glm::vec3(0.0f,0.0f,0.0f));
    Arcball arcball(&camera);
    arcball.enable(window);
    Scene theScene{};
    Renderer renderer(&theScene,&program);
    std::shared_ptr<ArbitraryShape> shape = std::shared_ptr<ArbitraryShape>(new ArbitraryShape(positions, faces));
    renderer.addMesh(shape);
    renderer.buildandrender(window, &camera, &theScene);
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
    const GLubyte* version = glGetString(GL_VERSION);
    std::cout << "OpenGL Version: " << version << std::endl;
    glViewport(0, 0, Renderer::screen_width, Renderer::screen_height);
    objFileInterpeter(window);

    glfwTerminate();
    return 0;
}
