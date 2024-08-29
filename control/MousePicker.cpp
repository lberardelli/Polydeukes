//
//  mouse.cpp
//  LearnOpenGLProject1
//
//  Created by Lawrence Berardelli on 2024-02-16.
//

#include <stdio.h>

#include "MousePicker.h"

int MousePicker::mousePositionX{};
int MousePicker::mousePositionY{};
Ray MousePicker::ray{};
Renderer* MousePicker::renderer{};
Camera* MousePicker::camera{};
Scene* MousePicker::theScene{};
Shape* MousePicker::targetShape{};
Arcball* MousePicker::arcball{};
