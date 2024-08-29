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

class vector {
  
public:
    static glm::vec3 HalfAngleVector(glm::vec3& light, glm::vec3& eye) {
        return glm::normalize((glm::normalize(light) + glm::normalize(eye)) / 2.0f);
    }
};


#endif /* vector_h */
