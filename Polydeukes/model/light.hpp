//
//  light.hpp
//  LearnOpenGLProject1
//
//  Created by Lawrence Berardelli on 2023-11-26.
//

#ifndef light_hpp
#define light_hpp

#include <stdio.h>
#include <glm.hpp>

class light {
private:
    
public:
    light(glm::vec3 colour, glm::vec3 position) : colour{colour}, position{position} {}
    
    glm::vec3 colour;
    glm::vec3 position;
    
};

#endif /* light_hpp */
