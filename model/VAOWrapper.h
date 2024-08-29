//
//  VAOWrapper.h
//  LearnOpenGLProject1
//
//  Created by Lawrence Berardelli on 2024-02-25.
//

#ifndef VAOWrapper_h
#define VAOWrapper_h

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class VAOWrapper {
  
private:
    int VAO;
    bool bound;
    
public:
    
    VAOWrapper() {
        glGenVertexArrays(1, VAO);
    }
    
    /*
     Will not call bind if already bound.
     */
    void bind() {
        if (!bound) {
            glBindVertexArray(VAO);
        }
        bound = true;
    }
    
    void unbind() {
        bound = false;
        glBindVertexArray(0);
    }
};

#endif /* VAOWrapper_h */
