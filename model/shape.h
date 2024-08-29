//
//  shape.h
//  LearnOpenGLProject1
//
//  Created by Lawrence Berardelli on 2024-02-25.
//

#ifndef shape_h
#define shape_h

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <vector>
#include "clickable.h"
#include "mesh.h"
#include "texture.h"
#include "../control/ShaderProgram.h"
#include <glad/glad.h>

#include <limits>

class ShaderProgram;
class SceneGraphNode;


/*
 A Shape object models the vertex data of the shape it represents and
 the modeling transformation it is subject to. It also has a shader program to enable swapping
 of shaders on the fly.
 
 Leaking a reference to a shader program in the model is questionable. But it's best so far.
 
 All shapes have all schema in common except the behaviour of the render call.
 So, we can make a shape builder and override a clone of Shape.
 
 Actually, even the render call can be shared. We can have a concrete generic shape that exposes a Shape(Mesh) constructor.
 Downstream this means that, short of any needed customizations, any concrete subclasses of shape are simply preloaded meshes.
 The mesh needs indices, the render call just uses them as draw elements call.
 AABB might be a member var to save time. Some design considerations here (could justify shape subclasses)
 */

class ShapeBuilder;

class Shape : public Clickable<Shape> {
    friend ShapeBuilder;
    friend SceneGraphNode;
protected:
    Mesh mesh;
    glm::mat4 modellingTransform = glm::mat4(1.0f);
    glm::vec3 colour = glm::vec3(1.0f, 1.0f, 1.0f);
    unsigned int texture = 0;
    unsigned int AABBVAO, AABBEBO, AABBVBO;
  
public:
    
    Shape() {
        glGenVertexArrays(1, &AABBVAO);
        glGenBuffers(1, &AABBEBO);
        glGenBuffers(1, &AABBVBO);
    }
    
    glm::mat4 getModellingTransform() const {
        return modellingTransform;
    }
    
    glm::vec3 getPosition() const {
        glm::vec4 tmp = glm::vec4(0.0f,0.0f,0.0f,1.0f);
        tmp = modellingTransform * tmp;
        return glm::vec3(tmp.x,tmp.y,tmp.z);
    }
    
    virtual void render(ShaderProgram shaderProgram) = 0;
    
    void renderAABB(std::vector<glm::vec3> aabb, ShaderProgram program) {
        glm::mat4 ide = glm::mat4(1.0f);
        program.setMat4("model", ide);
        //define the aabb mesh
        glm::vec3 blb = aabb[0];
        glm::vec3 trf = aabb[1];
        glm::vec3 brf(trf.x, blb.y, trf.z); // Bottom-right front
        glm::vec3 tlf(blb.x, trf.y, trf.z); // Top-left front
        glm::vec3 blf(blb.x, blb.y, trf.z); // Bottom-left front
        glm::vec3 trb(trf.x, trf.y, blb.z); // Top-right back
        glm::vec3 brb(trf.x, blb.y, blb.z); // Bottom-right back
        glm::vec3 tlb(blb.x, trf.y, blb.z); // Top-left back
        
        std::vector<glm::vec3> vertices = { blf, brf, tlf, trf, blb, brb, tlb, trb };
        float verticesArray[24]{};
        for (int i = 0; i < vertices.size(); ++i) {
            verticesArray[i*3] = vertices[i].x;
            verticesArray[i*3 + 1] = vertices[i].y;
            verticesArray[i*3 + 2] = vertices[i].z;
        }
        unsigned int indices[36] = {
            0, 1, 2, 3, 2, 1,  // Front face
            4, 5, 6, 7, 6, 5,  // Back face
            0, 4, 6, 2, 6, 0,  // Left face
            1, 3, 5, 7, 3, 5,  // Right face
            0, 1, 5, 5, 4, 0,  // Bottom face
            2, 3, 6, 6, 7, 3 // Top face
        };
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBindVertexArray(AABBVAO);
        glBindBuffer(GL_ARRAY_BUFFER, AABBVBO);
        glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), verticesArray, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, AABBEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindVertexArray(0);
    }
    
    virtual Shape* clone() = 0;
    
    virtual void onHover() override {
        hoverCallback(this);
    }
    
    virtual void offHover() override {
        offHoverCallback(this);
    }
    
    virtual void setModelingTransform(glm::mat4&& transform) {
        //there is no memory allocated for the model position, it is just world, so we get back to the model position (origin) like this
        glm::mat4 meshTransform = transform * glm::inverse(modellingTransform);
        mesh.updatePosition(meshTransform);
        modellingTransform = transform;
    }
    
    virtual void updateModellingTransform(glm::mat4&& transform) {
        setModelingTransform(glm::mat4(transform * modellingTransform));
    }
    
    virtual void addRotationTransform(glm::mat4&& rotation) {
        setModelingTransform(modellingTransform * rotation);
    }
    
    Shape(const Shape& that) {
        this->mesh = Mesh(that.mesh);
        this->modellingTransform = that.modellingTransform;
        this->colour = that.colour;
        this->texture = that.texture;
        this->clickCallback = that.clickCallback;
        this->hoverCallback = that.hoverCallback;
        this->offHoverCallback = that.offHoverCallback;
        glGenVertexArrays(1, &AABBVAO);
        glGenBuffers(1, &AABBEBO);
        glGenBuffers(1, &AABBVBO);
    }
    
    virtual void onClick() override {
        clickCallback(this);
    }
    
    /*
     To match the rest of the "customization over implementation" idea for these shapes, we need this to compute an AABB
     For any mesh. In fact, maybe the shape class is more a particle class, cause some shapes may not be clickable, e.g....
     Either way, we want compile time customization for generic shapes... back to the mesh's have aabb data idea
     */
    virtual std::vector<glm::vec3> getAABB() {
        std::vector<glm::vec3> positions = mesh.getPosition();
        return Shape::computeAABB(positions);
    }
    
    virtual std::vector<glm::vec3> getPositions() {
        std::vector<glm::vec3> positions = mesh.getPosition();
        return positions;
    }
    
    static std::vector<glm::vec3> computeAABB(std::vector<glm::vec3>& positions) {
        if (positions.size() == 0) {
            return std::vector<glm::vec3>({glm::vec3(0.0f,0.0f,0.0f)});
        }
        float minx = std::numeric_limits<float>::max();
        float miny = minx; float minz = minx;
        float maxx = -minx;
        float maxy = maxx; float maxz = maxx;
        for (auto pos : positions) {
            if (pos.x < minx) minx = pos.x;
            if (pos.y < miny) miny = pos.y;
            if (pos.z < minz) minz = pos.z;
            if (pos.x > maxx) maxx = pos.x;
            if (pos.y > maxy) maxy = pos.y;
            if (pos.z > maxz) maxz = pos.z;
        }

        if (std::abs(minx - maxx) < 0.1) maxx += 0.1f;
        if (std::abs(miny - maxy) < 0.1) maxy += 0.1f;
        if (std::abs(minz - maxz) < 0.1) maxz += 0.1f;

        std::vector<glm::vec3> aabb;
        aabb.push_back(glm::vec3(minx, miny, minz));
        aabb.push_back(glm::vec3(maxx, maxy, maxz));
        return aabb;
    }
    
    virtual void setColour(glm::vec3 colour) {
        this->colour = colour;
    }
    
    glm::vec3 getColour() const {
        return colour;
    }
    
    /*
     This method is also responsible for updating the aabb.
     */
    virtual void translate(glm::mat4& translation) {
        modellingTransform = translation * modellingTransform;
        mesh.updatePosition(translation);
    }
    
    void translate(glm::vec3& position) {
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), position);
        translate(translationMatrix);
    }
    
    virtual ~Shape() = default;
};

class ShapeFactory {
public:
    virtual Shape* build() = 0;
};

class ShapeBuilder {
private:
protected:
    Shape* shape;
public:
    ShapeBuilder& withTexture(std::string&& texture_path) {
        shape->texture = texture::generateTexture(texture_path);
        return *this;
    }
    
    ShapeBuilder& withColour(glm::vec3&& colour) {
        shape->colour = colour;
        return *this;
    }
    
    ShapeBuilder& withColour(glm::vec3& colour) {
        shape->colour = colour;
        return *this;
    }
    
    ShapeBuilder& withOnClickCallback(std::function<void(Shape*)> callback) {
        shape->setOnClick(callback);
        return *this;
    }
    
    ShapeBuilder& withPosition(glm::vec3& position) {
        shape->setModelingTransform(glm::translate(glm::mat4(1.0f), position));
        return *this;
    }
    
    ShapeBuilder& withPosition(glm::vec3&& position) {
        shape->setModelingTransform(glm::translate(glm::mat4(1.0f), position));
        return * this;
    }
    
    ShapeBuilder& withOnHoverCallback(std::function<void(Shape*)> callback) {
        shape->setOnHover(callback);
        return *this;
    }
    
    ShapeBuilder& withOffHoverCallback(std::function<void(Shape*)> callback) {
        shape->setOffHover(callback);
        return *this;
    }
    
    virtual Shape* build() {
        return shape->clone();
    }
};


#endif /* shape_h */
