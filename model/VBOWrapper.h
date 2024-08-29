//
//  VBOWrapper.h
//  LearnOpenGLProject1
//
//  Created by Lawrence Berardelli on 2024-02-25.
//

#ifndef VBOWrapper_h
#define VBOWrapper_h

#include <glad/glad.h>
#include <GLFW/glfw3.h>

/*
 Vertex Buffer Object wrapper to allow sharing of buffer between primitive shapes without rebinding.
 */
class VBOWrapper {
    
private:
    int VBO;
    bool bound;
    
public:
    VBOWrapper() {
        glGenBuffers(1,VBO);
    }
    
    void bind() {
        if (!bound) {
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
        }
        bound = true;
    }
    
    void unbind() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        bound = false;
    }
    
};

#endif /* VBOWrapper_h */
