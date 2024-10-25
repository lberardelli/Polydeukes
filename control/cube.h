//
//  cube.h
//  LearnOpenGLProject1
//
//  Created by Lawrence Berardelli on 2023-11-17.
//

#ifndef cube_h
#define cube_h


#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "../model/Camera.h"
#include "ShaderProgram.h"
#include "../model/light.hpp"
#include "vector.h"
#include "../model/cube.h"

class cube {
    
private:
    static float fieldOfView;
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        fieldOfView += yoffset;
    }
    
    
    float axies[12] = {
        0.0,0.0,0.0,
        1.0,0.0,0.0,
        0.0,1.0,0.0,
        0.0,0.0,1.0
    };
    
    float frustum[24] = {
        -0.05523, -0.0414, -0.1,    //(l,b,n)
        -0.05523, 0.0414, -0.1,     //(l,t,n)
        0.05523, -0.0414, -0.1,     //(r,b,n)
        0.05523, 0.0414, -0.1,      //(r,t,n)
        -55.23, -41.4, -100,        //(l,b,f)
        -55.23, 41.4, -100,         //(l,t,f)
        55.23, -41.4, -100,         //r,b,f)
        55.23, 41.4, -100           //(r,t,f)
    };
    
    unsigned int axisIndices[6] = {
        0,1,
        0,2,
        0,3
    };
    
    unsigned int frustumIndices[24] = {
        //front
        0,1,
        1,3,
        3,2,
        2,0,
        //back
        4,5,
        5,7,
        7,6,
        6,4,
        //connecting lines
        1,5,
        3,7,
        2,6,
        0,4
    };
    
public:
    
    void render(GLFWwindow* window, ShaderProgram shaderProgram) {}
};


#endif /* cube_h */
