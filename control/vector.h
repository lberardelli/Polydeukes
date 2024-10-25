//
//  vector.h
//  LearnOpenGLProject1
//
//  Created by Lawrence Berardelli on 2023-12-01.
//

#ifndef vector_h
#define vector_h

#include <glm.hpp>
#include <cmath>

struct Plane {
    glm::vec3 normal;
    glm::vec3 samplePoint;
    
    Plane(glm::vec3 normal, glm::vec3 samplePoint) : normal(normal), samplePoint(samplePoint) {}
};

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
};
    
class vector {
  
public:
    static glm::vec3 HalfAngleVector(glm::vec3& light, glm::vec3& eye) {
        return glm::normalize((glm::normalize(light) + glm::normalize(eye)) / 2.0f);
    }
    
    static glm::mat4 scaleGeometryBetweenTwoPointsTransformation(glm::vec3 currentPosition, glm::vec3 initialPosition) {
        glm::vec3 delta = currentPosition - initialPosition;
        glm::vec3 normalDelta = glm::normalize(delta);
        float deltaLength = glm::length(delta);
        float zval;
        if (normalDelta.z < 0.f) {
            zval = 1.0f;
        }
        else {
            zval = -1.0f;
        }
        glm::vec3 tmp = glm::normalize(glm::vec3(normalDelta.x, normalDelta.y, zval));
        glm::vec3 w = glm::cross(tmp, normalDelta);
        glm::vec3 v = glm::cross(w, normalDelta);
        glm::vec3 t = glm::vec3(0.f, 0.f, 0.f);
        glm::mat4 homogeneousMatrix = glm::mat4(
            glm::vec4(normalDelta, 0.0f),
            glm::vec4(v, 0.0f),
            glm::vec4(w, 0.0f),
            glm::vec4(t, 1.0f)
        );
        glm::mat4 scaleTransform = homogeneousMatrix * glm::scale(glm::mat4(1.0f), glm::vec3(deltaLength, 1.0, 1.0f));
        return glm::translate(glm::mat4(1.0f), (currentPosition - (glm::vec3(delta.x/2.f, delta.y/2.f, delta.z/2.0f)))) * scaleTransform;
    }
    
    static glm::vec3 rayPlaneIntersection(Plane plane, Ray ray) {
        float t = (glm::dot(plane.normal, plane.samplePoint) - glm::dot(plane.normal, ray.origin)) / glm::dot(plane.normal, ray.direction);
        return ray.origin + glm::vec3(t * ray.direction.x, t * ray.direction.y, t * ray.direction.z);
    }
    
    static glm::mat4 buildChangeOfBasisMatrix(glm::vec3 sourceVector) {
        glm::vec3 w = glm::normalize(sourceVector);
        glm::vec3 sample = sourceVector;
        float max = sourceVector.x;
        int i = 0;
        if (sourceVector.y > max) {
            max = sourceVector.y;
            i = 1;
        }
        if (sourceVector.z > max) {
            i = 2;
        }
        sample[i] = -1.0f * sample[i];
        glm::vec3 u = glm::cross(w, glm::normalize(sample));
        glm::vec3 v = glm::cross(w, u);
        double m[16] = {u[0],u[1],u[2],0,-v[0],-v[1],-v[2],0,-w[0],-w[1],-w[2], 0, 0,0,0,1};
        return glm::make_mat4(m);
    }
};


#endif /* vector_h */
