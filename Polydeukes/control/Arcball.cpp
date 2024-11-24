//
//  arcball.cpp
//  LearnOpenGLProject1
//
//  Created by Lawrence Berardelli on 2023-11-19.
//

#include <stdio.h>
#include "Arcball.h"

double Arcball::lastMouseX = 0;
double Arcball::lastMouseY = 0;
glm::vec3 Arcball::mouseDownVector = glm::vec3(0,0,1);
glm::quat Arcball::currentRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
glm::quat Arcball::previousRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
Camera* Arcball::cam{};
