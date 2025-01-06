//
//  particle.h
//  Polydeukes
//
//  Created by Lawrence Berardelli on 2024-05-19.
//

#ifndef particle_h
#define particle_h

#include "shape.h"
#include <functional>

#include <glm.hpp>
#include "../model/vector.h"

class Spring;
class Gravity;
class Drag;

class Particle {
    friend Spring;
    friend Drag;
    friend Gravity;
private:
    std::shared_ptr<Shape> shape;
    float inverse_mass;
    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 force = glm::vec3(0.0f);
    std::vector<std::function<void(Particle&, float deltaTime)>> tensors{};
    
    void updateForce(glm::vec3& force) {
        this->force += force;
    }
    
public:
    
    Particle(std::shared_ptr<Shape> shape, float inverse_mass) : shape(shape), inverse_mass(inverse_mass) {}
    
    Particle(std::shared_ptr<Shape> shape, float inverse_mass, glm::vec3 velocity) : shape(shape), inverse_mass(inverse_mass), velocity(velocity) {}
    
    Particle(const Particle& that) {
        this->shape = that.shape;
        inverse_mass = that.inverse_mass;
        velocity = that.velocity;
        for (auto tensor : that.tensors) {
            tensors.push_back(tensor);
        }
    }
        
    std::shared_ptr<Shape> getShape() {
        return shape;
    }
    
    /*
     Eventually we'll reverse this dependency and have the tensors be independent of the particles.
     And sorta just inject them into the update call directly.
     */
    void addTensor(std::function<void(Particle&,float deltaTime)> tensor) {
        tensors.push_back(tensor);
    }
    
    void basicCollision() {
        velocity = glm::vec3(velocity.x, -1 * velocity.y, velocity.z);
    }
    
    void update(float deltaTime) {
        for (auto tensor : tensors) {
            tensor(*this, deltaTime);
        }
        velocity += glm::vec3(force.x * inverse_mass * deltaTime, force.y * inverse_mass * deltaTime, force.z * inverse_mass * deltaTime);
        glm::vec3 deltaPosition = glm::vec3(velocity.x * deltaTime, velocity.y * deltaTime, velocity.z * deltaTime);
        shape->translate(deltaPosition);
        force = glm::vec3(0.0f,0.0f,0.0f);
    }
    
    glm::vec3 getVelocity() const {
        return velocity;
    }
    
    glm::vec3 getPosition() const {
        return shape->getPosition();
    }
    
};

class Gravity {
public:
    static void update(Particle& particle, float deltaTime) {
        glm::vec3 force = glm::vec3( 0.0f, -30.0f, 0.0f);
        particle.updateForce(force);
    }
};

class Spring {
private:
    float springConstant = 0;
    float restingLength;
    glm::vec3 initialPosition{};
    std::shared_ptr<Shape> coilShape{};
public:
    
    Spring(float constant, float restingLength, glm::vec3 initialPosition, std::shared_ptr<Shape> coilShape) : springConstant(constant), restingLength(restingLength), initialPosition(initialPosition), coilShape(coilShape) {}
    
    Spring(float constant, float restingLength) : springConstant(constant), restingLength(restingLength) {}
    
    glm::vec3 getFixedPosition() const {
        return initialPosition;
    }
    
    void update(Particle& particle, float deltaTime) {
        glm::vec3 currentPosition = particle.getPosition();
        glm::vec3 delta = currentPosition - initialPosition;
        float deltaLength = glm::length(delta);
        coilShape->setModelingTransform(vector::scalGeometryBetweenTwoPointsStretch(currentPosition, initialPosition));
        float magnitude = -1.0f * springConstant * (glm::length(delta) - restingLength);
        glm::vec3 direction = glm::normalize(delta);
        glm::vec3 force = glm::vec3(direction.x * magnitude, direction.y * magnitude, direction.z * magnitude);
        particle.updateForce(force);
    }
};

class Drag {
    
private:
    
    float k1, k2;
public:
    
    Drag(float k1, float k2) : k1(k1), k2(k2) {}
    
    void update(Particle& particle, float deltaTime) {
        if (glm::length(particle.getVelocity()) < 0.01) {
            return;
        }
        glm::vec3 velocity = particle.getVelocity();
        float velocity_magnitude = glm::length(velocity);
        float magnitude = -1.0f * (k1 * velocity_magnitude + k2 * velocity_magnitude * velocity_magnitude);
        glm::vec3 direction = glm::normalize(velocity);
        glm::vec3 force = glm::vec3(direction.x * magnitude, direction.y * magnitude, direction.z * magnitude);
        particle.updateForce(force);
    }
};


#endif /* particle_h */
