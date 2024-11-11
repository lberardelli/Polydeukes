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
#include "../control/ShaderProgram.h"

class objInterpreter {
private:
    class ArbitraryShape : public Shape {
    private:
        std::vector<float> vertices;
        std::vector<float> normals;
        std::vector<float> textureCoords;
        unsigned int VAO, VBO;
        
        ArbitraryShape(std::vector<float> vertices, unsigned int VAO, unsigned int VBO) : vertices(vertices), VAO(VAO), VBO(VBO) {}
    public:
        ArbitraryShape(std::vector<glm::vec3> positions, std::vector<std::vector<int>> faces) {
            for (auto face : faces) {
                glm::vec3 anchor = positions[face[0]];
                for (int i = 2; i < face.size(); ++i) {
                    glm::vec3 first = positions[face[i-1]];
                    glm::vec3 second = positions[face[i]];
                    glm::vec3 u = anchor - first;
                    glm::vec3 v = anchor - second;
                    glm::vec3 normal = glm::cross(u, v);
                    vertices.push_back(anchor.x);
                    vertices.push_back(anchor.y);
                    vertices.push_back(anchor.z);
                    vertices.push_back(normal.x);
                    vertices.push_back(normal.y);
                    vertices.push_back(normal.z);
                    
                    vertices.push_back(first.x);
                    vertices.push_back(first.y);
                    vertices.push_back(first.z);
                    vertices.push_back(normal.x);
                    vertices.push_back(normal.y);
                    vertices.push_back(normal.z);
                    
                    vertices.push_back(second.x);
                    vertices.push_back(second.y);
                    vertices.push_back(second.z);
                    vertices.push_back(normal.x);
                    vertices.push_back(normal.y);
                    vertices.push_back(normal.z);
                }
            }
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(float) * 6, (void*)0);
            glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(float) * 6, (void*)(3*sizeof(float)));
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glBindVertexArray(0);
        }
        
        void render(ShaderProgram shaderProgram) {
            shaderProgram.setMat4("model", modellingTransform);
            shaderProgram.setVec3("aColour", colour);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, (int)vertices.size() / 6);
        }
        
        std::shared_ptr<Shape> clone() {
            return std::shared_ptr<ArbitraryShape>(new ArbitraryShape(vertices, VAO, VBO));
        }
    };
    
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
        return shape;
    }
    
};


#endif /* objinterpreter_h */
