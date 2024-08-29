//
//  mesh.h
//  LearnOpenGLProject1
//
//  Created by Lawrence Berardelli on 2024-03-04.
//

#ifndef mesh_h
#define mesh_h

#include <glm.hpp>
#include <vector>

class Mesh {
  
private:
    std::vector<glm::vec3> positions{};
    std::vector<glm::vec3> normals{};
    std::vector<glm::vec2> textures{};
    
public:
    
    Mesh() {
        
    }
    
    Mesh(float* data, int size) {
        for (int i = 0; i < size; i += 8) {
            glm::vec3 position = glm::vec3(data[i], data[i+1], data[i+2]);
            glm::vec3 normal = glm::vec3(data[i+3], data[i+4], data[i+5]);
            glm::vec2 texture_coords = glm::vec2(data[i+6], data[i+7]);
            positions.push_back(position);
            normals.push_back(normal);
            textures.push_back(texture_coords);
        }
    }
    
    Mesh(const Mesh& mesh) {
        for (auto vec3 : mesh.positions) {
            positions.push_back(vec3);
        }
        for (auto vec3 : mesh.normals) {
            normals.push_back(vec3);
        }
        for (auto vec2 : mesh.textures) {
            textures.push_back(vec2);
        }
    }
    
    std::vector<glm::vec3> getPosition() {
        return positions;
    }
    
    void updatePosition(glm::mat4& transform) {
        std::vector<glm::vec3> newPositions{};
        for (auto pos : positions) {
            glm::vec4 tmp = glm::vec4(pos.x,pos.y,pos.z,1.0f);
            tmp = transform * tmp;
            newPositions.push_back(glm::vec3(tmp.x,tmp.y,tmp.z));
        }
        positions = newPositions;
    }
    
};


#endif /* mesh_h */
