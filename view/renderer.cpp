//
//  renderer.cpp
//  Polydeukes
//
//  Created by Lawrence Berardelli on 2024-04-27.
//

#include "renderer.h"

unsigned int Renderer::screen_width = 1600;
unsigned int Renderer::screen_height = 900;

bool Chip8InputHandler::is_key_pressed[16] = {
    false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false
};
