//
//  cube.h
//  LearnOpenGLProject1
//
//  Created by Lawrence Berardelli on 2024-02-25.
//

#ifndef cube_hpp
#define cube_hpp

#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shape.h"
#include "ShaderProgram.h"
#include "mesh.h"
#include <vector>

class CubeFactory;

/*
 At first shapes in the scene will assume some rendering responsibilities.
 
 First issue with the idea of adding a VAO to each model class is rendering the same shape
 multiple times subject to simple modeling transforms. At first trying to have a single VAO for each basic shape.
 
 On a conceptual level this abstraction is leaking into opengl specifics, a bit on purpose a bit not-so-ideal
 as in the case of the shaderProgram reference. But that solution offers maximum control with minimum overhead
 other than polluting the Cube model... which is acceptable I think.
 
 I think if we someday want to support a custom shader attached to an object we need to make a manager for it.
 For now just pass in the shader and add the modelling transform.
 */
class Cube : public Shape {
    friend CubeFactory;
private:
    /*
     OPENGL specific
     */
    unsigned int VAO;
    
    
    Cube(unsigned int& vao, float* vertices) : VAO(vao) {
        mesh = Mesh(vertices, 216);
    }
    
    Cube(Cube& that) : Shape(that), VAO(that.VAO) {}
        
public:
    
    virtual ~Cube() = default;
    
    /*
     This will probably be taken out when on-the-fly shader program changes are necessary. (Seems like this is a classic view component).
     Idea: A container that wraps an shape and a shader program...
     Idea: it's better to just inject the shader and shape as a callback to a render method in the view.
     Anyway for now this leaky thing will have to do.
     
     For every draw call a renderer needs to know:
     what geometry (vertex data) will I draw?
     what textures and shaders will I use?
     what transforms will I use?
     what other graphics state will I set (does this draw need blending, etc)?
     
     Can use a render descriptor object to contain those data for each object. Same problem of how
     to associate render descriptor data with a given object.
     
     "Obvious" problem with current design is now the shader must have a "model" uniform.
     
     The shader and the mesh data are coupled anyway, as the shader needs to know what the format of
     the incoming data is.
     */
    virtual void render(ShaderProgram shaderProgram) override {
        shaderProgram.setMat4("model", modellingTransform);
        shaderProgram.setVec3("aColour", colour);
        if (texture) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
        }
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //Shape::renderAABB(getAABB(), shaderProgram);
    }
    
    std::shared_ptr<Shape> clone() override {
        auto retval =  std::shared_ptr<Cube>(new Cube(*this));
        retval->referenceToThis = retval;
        return retval;
    }
    
};

/*
 Using a factory here because I don't want to make a new VAO and bind it foreach cube.
 More efficient to make them share a VAO/VBO combo.
 */
class CubeFactory : ShapeFactory {
private:
    unsigned int VAO;
    unsigned int VBO;
    
    float vertices[288] =
    {
        // Triangle 1 - Front face
        -0.5, -0.5, -0.5,  0,  0, -1,  0, 0,  // Vertex 0 + Normal + Texture
        0.5, -0.5, -0.5,  0,  0, -1,   1, 0,  // Vertex 1 + Normal + Texture
        0.5,  0.5, -0.5,  0,  0, -1,   1, 1,  // Vertex 2 + Normal + Texture

        // Triangle 2 - Front face
        -0.5, -0.5, -0.5,  0,  0, -1,   0, 0,  // Vertex 0 + Normal + Texture
        0.5,  0.5, -0.5,  0,  0, -1,   1, 1,  // Vertex 2 + Normal + Texture
        -0.5,  0.5, -0.5,  0,  0, -1,   0, 1,  // Vertex 3 + Normal + Texture

        // Triangle 3 - Back face
        -0.5, -0.5,  0.5,  0,  0,  1,   0, 0,  // Vertex 4 + Normal + Texture
        0.5, -0.5,  0.5,  0,  0,  1,   1, 0,  // Vertex 5 + Normal + Texture
        0.5,  0.5,  0.5,  0,  0,  1,   1, 1,  // Vertex 6 + Normal + Texture

        // Triangle 4 - Back face
        -0.5, -0.5,  0.5,  0,  0,  1,   0, 0,  // Vertex 4 + Normal + Texture
        0.5,  0.5,  0.5,  0,  0,  1,   1, 1,  // Vertex 6 + Normal + Texture
        -0.5,  0.5,  0.5,  0,  0,  1,   0, 1,  // Vertex 7 + Normal + Texture

        // Triangle 5 - Left face
        -0.5, -0.5, -0.5, -1,  0,  0,   0, 0,  // Vertex 0 + Normal + Texture
        -0.5,  0.5, -0.5, -1,  0,  0,   1, 0,  // Vertex 3 + Normal + Texture
        -0.5,  0.5,  0.5, -1,  0,  0,   1, 1,  // Vertex 7 + Normal + Texture

        // Triangle 6 - Left face
        -0.5, -0.5, -0.5, -1,  0,  0,   0, 0,  // Vertex 0 + Normal + Texture
        -0.5,  0.5,  0.5, -1,  0,  0,   1, 1,  // Vertex 7 + Normal + Texture
        -0.5, -0.5,  0.5, -1,  0,  0,   0, 1,  // Vertex 4 + Normal + Texture

        // Triangle 7 - Right face
        0.5, -0.5, -0.5,  1,  0,  0,   0, 0,  // Vertex 1 + Normal + Texture
        0.5,  0.5, -0.5,  1,  0,  0,   1, 0,  // Vertex 2 + Normal + Texture
        0.5,  0.5,  0.5,  1,  0,  0,   1, 1,  // Vertex 6 + Normal + Texture

        // Triangle 8 - Right face
        0.5, -0.5, -0.5,  1,  0,  0,   0, 0,  // Vertex 1 + Normal + Texture
        0.5,  0.5,  0.5,  1,  0,  0,   1, 1,  // Vertex 6 + Normal + Texture
        0.5, -0.5,  0.5,  1,  0,  0,   0, 1,  // Vertex 5 + Normal + Texture

        // Triangle 9 - Bottom face
        -0.5, -0.5, -0.5,  0, -1,  0,   0, 0,  // Vertex 0 + Normal + Texture
        0.5, -0.5, -0.5,  0, -1,  0,   1, 0,  // Vertex 1 + Normal + Texture
        0.5, -0.5,  0.5,  0, -1,  0,   1, 1,  // Vertex 5 + Normal + Texture

        // Triangle 10 - Bottom face
        -0.5, -0.5, -0.5,  0, -1,  0,  0, 0,  // Vertex 0 + Normal + Texture
        0.5, -0.5,  0.5,  0, -1,  0,   1, 1,  // Vertex 5 + Normal + Texture
        -0.5, -0.5,  0.5,  0, -1, 0, 0, 1,   // Vertex 4 + Normal + Texture
        
        // Triangle 11 - Top face
        -0.5, 0.5, -0.5, 0, 1, 0, 0, 0, // Vertex 3 + Normal + Texture
        0.5, 0.5, -0.5, 0, 1, 0, 1, 0, // Vertex 2 + Normal + Texture
        0.5, 0.5, 0.5, 0, 1, 0, 1, 1, // Vertex 6 + Normal + Texture
        
        //Triangle 12 - Top face
        -0.5, 0.5, -0.5, 0, 1, 0, 0, 0, // Vertex 3 + Normal + Texture
        0.5, 0.5, 0.5, 0, 1, 0, 1, 1, // Vertex 6 + Normal + Texture
        -0.5, 0.5, 0.5, 0, 1, 0, 0, 1 // Vertex 7 + Normal + Texture
    };
    
public:
    
    CubeFactory() {
        glGenVertexArrays(1,&VAO);
        glGenBuffers(1,&VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
        glEnableVertexAttribArray(2);
    }
    
    std::shared_ptr<Shape> build() {
        return std::shared_ptr<Cube>(new Cube(VAO, vertices));
    }
    
};

class CubeBuilder : public ShapeBuilder {
private:
    static CubeFactory* cubeFactory;
    
public:
    
    CubeBuilder() {
        if (!cubeFactory) {
            cubeFactory = new CubeFactory();
        }
        shape = cubeFactory->build();
    }
    
    virtual std::shared_ptr<Shape> build() {
        return shape->clone();
    }

};

#endif /* cube_hpp */
