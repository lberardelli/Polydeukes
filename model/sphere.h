//
//  sphere.h
//  Polydeukes
//
//  Created by Lawrence Berardelli on 2024-04-26.
//

#ifndef sphere_h
#define sphere_h

#include "shape.h"
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "texture.h"
#include <memory>


class SphereFactory;
class SphereBuilder;

class Sphere : public Shape
{
    friend SphereFactory;
    friend SphereBuilder;
    
protected:
    unsigned int VAO;
    unsigned int nIndices;
    
    Sphere(Sphere& that) : Shape(that), VAO(that.VAO), nIndices(that.nIndices) {
    }
    
    Sphere(unsigned int VAO, float* vertices, int nVertices, int nIndices) : VAO(VAO), nIndices(nIndices) {
        mesh = Mesh(vertices, nVertices);
    }
    
public:
    
    virtual ~Sphere() = default;
    
    std::unique_ptr<Shape> clone() override {
        return std::unique_ptr<Sphere>(new Sphere(*this));
    }
    
    void render(ShaderProgram shaderProgram) override {
        shaderProgram.setMat4("model", modellingTransform);
        shaderProgram.setVec3("aColour", colour);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
        //Shape::renderAABB(getAABB(), shaderProgram);
    }
    
};

class SphereFactory : public ShapeFactory
{
    friend SphereBuilder;
private:
    
    std::vector<float> vertices;
    std::vector<int> indices;
    unsigned int nIndices;
    unsigned int VAO;
    unsigned int nvertices;
    
    SphereFactory(float radius, float stackCount, float sectorCount) {
        generateSphereData(radius, stackCount, sectorCount);
        generateIndices(stackCount, sectorCount);
        // create VAO to store all vertex array state to VAO
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // create VBO to copy interleaved vertex data (V/N/T) to VBO
        GLuint vboId;
        glGenBuffers(1, &vboId);
        glBindBuffer(GL_ARRAY_BUFFER, vboId);           // for vertex data
        glBufferData(GL_ARRAY_BUFFER,                   // target
                     nvertices * sizeof(float), // data size, # of bytes
                     vertices.data(),   // ptr to vertex data
                     GL_STATIC_DRAW);                   // usage

        // create VBO to copy index data to VBO
        GLuint iboId;
        glGenBuffers(1, &iboId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);   // for index data
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,           // target
                     nIndices * sizeof(int),             // data size, # of bytes
                     indices.data(),               // ptr to index data
                     GL_STATIC_DRAW);                   // usage

        // activate attrib arrays
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        // set attrib arrays with stride and offset
        glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(float) * 8, (void*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(float) * 8, (void*)(sizeof(float)*3));
        glVertexAttribPointer(2,  2, GL_FLOAT, false, sizeof(float) * 8, (void*)(sizeof(float)*6));

        // unbind VAO and VBOs
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    
    // Thank you https://www.songho.ca/opengl/gl_sphere.html
    void generateSphereData(float radius, float stackCount, float sectorCount) {
        float x, y, z, xy;                              // vertex position
        float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
        float s, t;
        float PI = 3.14159;
        // vertex texCoord

        float sectorStep = 2 * PI / sectorCount;
        float stackStep = PI / stackCount;
        float sectorAngle, stackAngle;
        nvertices = stackCount * sectorCount * 8;
        for(int i = 0; i <= stackCount; ++i)
        {
            stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
            xy = radius * cosf(stackAngle);             // r * cos(u)
            z = radius * sinf(stackAngle);              // r * sin(u)

            // add (sectorCount+1) vertices per stack
            // first and last vertices have same position and normal, but different tex coords
            for(int j = 0; j <= sectorCount; ++j)
            {
                sectorAngle = j * sectorStep;           // starting from 0 to 2pi

                // vertex position (x, y, z)
                x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
                y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);

                // normalized vertex normal (nx, ny, nz)
                nx = x * lengthInv;
                ny = y * lengthInv;
                nz = z * lengthInv;
                vertices.push_back(nx);
                vertices.push_back(ny);
                vertices.push_back(nz);

                // vertex tex coord (s, t) range between [0, 1]
                s = (float)j / sectorCount;
                t = (float)i / stackCount;
                vertices.push_back(s);
                vertices.push_back(t);
            }
        }
        nvertices = vertices.size();
    }
    
    void generateIndices(int stackCount, int sectorCount) {
        int k1, k2;
        for(int i = 0; i < stackCount; ++i)
        {
            k1 = i * (sectorCount + 1);     // beginning of current stack
            k2 = k1 + sectorCount + 1;      // beginning of next stack

            for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
            {
                // 2 triangles per sector excluding first and last stacks
                // k1 => k2 => k1+1
                if(i != 0)
                {
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1 + 1);
                }

                // k1+1 => k2 => k2+1
                if(i != (stackCount-1))
                {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                }
            }
        }
        nIndices = indices.size();
    }
    
    std::unique_ptr<Shape> build() override {
        return std::unique_ptr<Sphere>(new Sphere(VAO, vertices.data(), nvertices, nIndices));
    }
    
};

class SphereBuilder : public ShapeBuilder {
private:
    static SphereFactory* sphereFactory;
    static SphereBuilder* instance;
    
    SphereBuilder() {
        if (!sphereFactory) {
            sphereFactory = new SphereFactory(0.5, 35, 35);
        }
        shape = sphereFactory->build();
    }
    
    virtual ~SphereBuilder() {
        delete sphereFactory;
        delete instance;
    }
    
public:
    
    static SphereBuilder* getInstance() {
        if (!instance) {
            instance = new SphereBuilder();
        }
        return instance;
    }
    
    std::unique_ptr<Shape> build() override {
        return shape->clone();
    }
    
};


#endif /* sphere_h */
