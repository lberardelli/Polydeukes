//
//  camera.h
//  LearnOpenGLProject1
//
//  Created by Lawrence Berardelli on 2023-11-25.
//

#ifndef camera_h
#define camera_h

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

class Camera {
    
private:
    
    glm::vec3 position;
    glm::vec3 z_axis,x_axis,y_axis;
    glm::mat4 view;
    glm::mat4 canonicalTransform;
    glm::mat4 arcballTransform = glm::mat4(1.0f);
    
public:
  
    glm::mat4 viewingTransformation() const {
        return view;
    }
    
    Camera() {
        position = glm::vec3(0.0f,10.0f,35.0f);
        //lookat basis
        z_axis = glm::normalize(position - glm::vec3(0.0f,0.0f,0.0f));
        x_axis = glm::normalize(glm::cross(glm::vec3(0.0f,1.0f,0.0f), z_axis));
        y_axis = glm::cross(z_axis, x_axis);
        //translate then rotate to canonical basis
        double m[16] = {x_axis[0],x_axis[1],x_axis[2],0,y_axis[0],y_axis[1],y_axis[2],0,z_axis[0],z_axis[1],z_axis[2],0,position[0],position[1],position[2],1};
        canonicalTransform = glm::make_mat4(m);
        view = glm::inverse(canonicalTransform);
    }
    
    void updateViewingTransformation(glm::mat4&& transform) {
        arcballTransform = transform;
        view = glm::inverse(canonicalTransform) * transform;
    }
    
    glm::vec3 getPosition() {
        glm::vec4 tmp = glm::vec4(position, 1.0f) * arcballTransform;
        return tmp;
    }
    
    glm::vec3 getUpVector() {
        return glm::vec4(y_axis, 0.0f) * arcballTransform;
    }
    
    glm::vec3 getDirection() {
        return glm::vec4(-z_axis, 0.0f) * arcballTransform;
    }
    
    glm::mat4 arcballTransformation() {
        return arcballTransform;
    }

};


#endif /* camera_h */
