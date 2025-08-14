//
//  armature.h
//  Polydeukes
//
//  Created by Lawrence Berardelli on 2025-04-23.
//

#ifndef armature_h
#define armature_h

#include "shape.h"
#include "sphere.h"
#include "cube.h"
#include "vector.h"
#include "Camera.h"

#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <memory>

class Armature : public Shape, public std::enable_shared_from_this<Armature> {
    
private:
    
    std::shared_ptr<Shape> head;
    std::shared_ptr<Shape> body;
    std::shared_ptr<Shape> tail;
    glm::vec3 startPosition = glm::vec3(0.f,0.f,0.f);
    glm::vec3 endPosition = glm::vec3(0.f,5.f,0.f);
    
public:
    
    std::vector<glm::vec3> getTailRegion() const {
        return {glm::vec3(startPosition.x - .51f, startPosition.y - .51f, startPosition.z - .51f), glm::vec3(startPosition.x + .51f, startPosition.y + .51f, startPosition.z + .51f)};
    }
    
    std::vector<glm::vec3> getHeadRegion() const {
        return {glm::vec3(endPosition.x - .51f, endPosition.y - .51f, endPosition.z - .51f), glm::vec3(endPosition.x + .51f, endPosition.y + .51f, endPosition.z + .51f)};
    }
    
    static void armatureClickCallback(GLFWwindow* window, Camera* camera, std::shared_ptr<Shape> shape, glm::vec3 exactPosition) {
        std::shared_ptr<Armature> armature = std::dynamic_pointer_cast<Armature>(shape);
        //if the exact position is on the sphere then adjust the bearing
        if (exactPosition.x < armature->endPosition.x + 0.51f && exactPosition.x > armature->endPosition.x - 0.51f &&
            exactPosition.y < armature->endPosition.y + 0.51f && exactPosition.y > armature->endPosition.y - 0.51f &&
            exactPosition.z < armature->endPosition.z + 0.51f && exactPosition.z > armature->endPosition.z - 0.51f) {
            MeshDragger::camera = camera;
            MeshDragger::registerMousePositionCallback(window, armature->head);
        } else if (exactPosition.x < armature->startPosition.x + 0.51f && exactPosition.x > armature->startPosition.x - 0.51f &&
                   exactPosition.y < armature->startPosition.y + 0.51f && exactPosition.y > armature->startPosition.y - 0.51f &&
                   exactPosition.z < armature->startPosition.z + 0.51f && exactPosition.z > armature->startPosition.z - 0.51f) {
            MeshDragger::camera = camera;
            MeshDragger::registerMousePositionCallback(window, armature->tail);
        } else {
            MeshDragger::camera = camera;
            MeshDragger::registerMousePositionCallback(window, armature->body);
        }
        //if the exact position is on the cuboid then adjust the location.
    }
    
    Armature() {
        head = SphereBuilder::getInstance()->build();
        glm::mat4 model = glm::translate(glm::mat4(1.f), glm::vec3(0.f,5.f,0.f));
        head->setModelingTransform(model);
        body = CubeBuilder().build();
        tail = CubeBuilder().build();
        head->setOnMouseDrag([&](std::weak_ptr<Shape> theShape) {
            endPosition = theShape.lock()->getPosition();
            auto m2 = vector::scaleGeometryBetweenTwoPointsStretch(endPosition, startPosition);
            body->setModelingTransform(m2);
        });
        body->setOnMouseDrag([&](std::weak_ptr<Shape> theShape) {
            glm::vec3 d = endPosition - startPosition;
            glm::vec3 centre = theShape.lock()->getPosition();
            startPosition = glm::vec3(centre.x - d.x / 2.f, centre.y - d.y / 2.f, centre.z - d.z / 2.f);
            endPosition = glm::vec3(centre.x + d.x / 2.f, centre.y + d.y / 2.f, centre.z + d.z / 2.f);
            head->setModelingTransform(glm::translate(glm::mat4(1.f), endPosition));
            tail->setModelingTransform(glm::translate(glm::mat4(1.f), startPosition));
        });
        tail->setOnMouseDrag([&](std::weak_ptr<Shape> theShape) {
            startPosition = theShape.lock()->getPosition();
            auto m2 = vector::scaleGeometryBetweenTwoPointsStretch(endPosition, startPosition);
            body->setModelingTransform(m2);
        });
        glm::mat4 m2 = vector::scaleGeometryBetweenTwoPointsStretch(glm::vec3(0.f,5.f,0.f), glm::vec3(0.f,0.f,0.f));
        body->setModelingTransform(m2);
    }
    
    void initReferenceToThis() {
        referenceToThis = shared_from_this();
    }
    
    std::vector<glm::vec3> getPositions() override {
        std::vector<glm::vec3> retval{};
        std::vector<glm::vec3> a = head->getPositions();
        std::vector<glm::vec3> b = body->getPositions();
        std::vector<glm::vec3> c = tail->getPositions();
        retval.insert(retval.end(), a.begin(), a.end());
        retval.insert(retval.end(), b.begin(), b.end());
        retval.insert(retval.end(), c.begin(), c.end());
        return retval;
    }
    
    void render(ShaderProgram shaderProgram) override {
        head->render(shaderProgram);
        body->render(shaderProgram);
        tail->render(shaderProgram);
    }
    
    void setColour(glm::vec3 colour) override {
        head->setColour(colour);
        body->setColour(colour);
        tail->setColour(colour);
    }
    
    std::shared_ptr<Shape> clone() override {
        return nullptr;
    }
    
};


#endif /* armature_h */
