//
//  objinterpreter.h
//  Polydeukes
//
//  Created by Lawrence Berardelli on 2024-11-11.
//

#ifndef objinterpreter_h
#define objinterpreter_h

#include "shape.h"
#include <glad/glad.h>
#include "ShaderProgram.h"
#include "../model/vector.h"

#include <memory>

class ArbitraryShape : public Shape, public std::enable_shared_from_this<ArbitraryShape> {
private:
    unsigned int VAO, VBO;
    unsigned int nVertices = 0;
    ArbitraryShape(ArbitraryShape& that) : Shape(that), VAO(that.VAO), VBO(that.VBO), nVertices(that.nVertices) {}
    
    void init(std::vector<Triangle> positions) {
        std::vector<float> vertices;
        for (auto triangle : positions) {
            glm::vec3 u = triangle.a - triangle.b;
            glm::vec3 v = triangle.a - triangle.c;
            glm::vec3 normal = glm::cross(u, v);
            vertices.push_back(triangle.a.x);
            vertices.push_back(triangle.a.y);
            vertices.push_back(triangle.a.z);
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);
            vertices.push_back(0.f);
            vertices.push_back(0.f);
            
            vertices.push_back(triangle.b.x);
            vertices.push_back(triangle.b.y);
            vertices.push_back(triangle.b.z);
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);
            vertices.push_back(0.f);
            vertices.push_back(0.f);
            
            vertices.push_back(triangle.c.x);
            vertices.push_back(triangle.c.y);
            vertices.push_back(triangle.c.z);
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);
            vertices.push_back(0.f);
            vertices.push_back(0.f);
            nVertices+=3;
        }
        mesh = Mesh(vertices.data(), (int)vertices.size());
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(float) * 8, (void*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(float) * 8, (void*)(3*sizeof(float)));
        glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(float) * 8, (void*)(6*sizeof(float)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glBindVertexArray(0);
    }
public:
    
    virtual std::vector<glm::vec3> getAABB() override {
        return {glm::vec3(0.f,0.f,0.f), glm::vec3(0.f,0.f,0.f)};
    }
    
    explicit ArbitraryShape(std::vector<Triangle> positions) {
        init(positions);
    }
    
    //TODO: Rectify this disaster. 
    void initReferenceToThis() {
        referenceToThis = shared_from_this();
    }
    
    ArbitraryShape(std::vector<glm::vec3> positions, std::vector<std::vector<int>> faces) {
        colour = glm::vec4(1.0f,1.0f,1.0f,.5f);
        std::vector<Triangle> triangles;
        for (auto face : faces) {
            glm::vec3 anchor = positions[face[0]];
            for (int i = 2; i < face.size(); ++i) {
                glm::vec3 first = positions[face[i-1]];
                glm::vec3 second = positions[face[i]];
                Triangle triangle(anchor, first, second);
                triangles.push_back(triangle);
            }
        }
        init(triangles);
    }
    
    void render(ShaderProgram shaderProgram) override {
        shaderProgram.setMat4("model", modellingTransform);
        shaderProgram.setVec3("aColour", colour);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, nVertices);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        //std::vector<glm::vec3> positions = mesh.getPosition();
        //renderAABB(computeAABB(positions), shaderProgram);
    }
    
    std::shared_ptr<Shape> clone() override {
        auto retval = std::shared_ptr<ArbitraryShape>(new ArbitraryShape(*this));
        retval->referenceToThis = retval;
        return retval;
    }
};

class objInterpreter {
private:
    
    static std::vector<int> parseFaceLine(std::string delim, std::string line) {
        std::vector<std::string> tokens;
        std::stringstream ss(line);
        std::string token;
        while (std::getline(ss, token, ' ')) {
            if (delim != "") {
                token = token.substr(0, token.find(delim, 0));
            }
            tokens.push_back(token);
        }
        std::vector<int> face{};
        for (int i = 1; i < tokens.size(); ++i) {
            face.push_back(std::stoi(tokens[i])-1);
        }
        return face;
    }
    
public:
    static std::shared_ptr<Shape> interpretObjFile(std::string objFile) {
        std::ifstream inputFile(objFile);
        if (!inputFile) {
            std::cerr << "Failed to open the file " << objFile << std::endl;
            return std::shared_ptr<Shape>(nullptr);
        }
        std::string line{};
        std::vector<glm::vec3> positions{};
        std::vector<std::vector<int>> faces{};
        
        while (std::getline(inputFile,line)) {
            if (line.length() == 0) {
                continue;
            }
            if (line.at(0) == '#') {
                continue;
            }
            if (line.at(0) == 'v' && line.at(1) == ' ') {
                std::vector<std::string> tokens;
                std::stringstream ss(line);
                std::string token;
                while (std::getline(ss, token, ' ')) {
                    tokens.push_back(token);
                }
                glm::vec3 position = glm::vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
                positions.push_back(position);
                continue;
            }
            if (line.at(0) == 'f') {
                if (line.find('/') == std::string::npos)
                {
                    faces.push_back(parseFaceLine("", line));
                }
                else if (line.find("//") != std::string::npos)
                {
                    faces.push_back(parseFaceLine("//", line));
                }
                else if (std::count(line.begin(), line.end(), '/') == 3)
                {
                    faces.push_back(parseFaceLine("/", line));
                }
                else
                {
                    faces.push_back(parseFaceLine("/", line));
                }
            }
        }
        std::shared_ptr<ArbitraryShape> shape = std::shared_ptr<ArbitraryShape>(new ArbitraryShape(positions, faces));
        shape->initReferenceToThis();
        return shape;
    }
    
};


#endif /* objinterpreter_h */
