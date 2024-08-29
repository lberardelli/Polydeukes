//
//  Scene.h
//  LearnOpenGLProject1
//
//  Created by Lawrence Berardelli on 2024-02-26.
//

#ifndef Scene_h
#define Scene_h

#include <vector>
#include <memory>
#include <algorithm>
#include <GLFW/glfw3.h>
#include <glm.hpp>

#include "shape.h"

class SceneGraphNode {
public:
    Shape* data;
    SceneGraphNode* next = 0;
    
    SceneGraphNode(SceneGraphNode& that) {
        data = that.data->clone();
        if (that.next) {
            next = new SceneGraphNode(*that.next);
        }
    }
    
    SceneGraphNode() = default;
    
    ~SceneGraphNode() {
        delete data;
    }
    
};

class Scene;


class SceneGraph : public Shape {
    friend Scene;
private:
    SceneGraphNode* head = 0;
    void dtor_helper_rec(SceneGraphNode* tmp) {
        if (tmp->next) {
            dtor_helper_rec(tmp->next);
        }
        delete tmp;
    }
    
public:
    
    SceneGraph(const SceneGraph& that) : Shape(that) {
        if (that.head) {
            this->head = new SceneGraphNode(*that.head);
        }
    }
    
    void setColour(glm::vec3 colour) override {
        SceneGraphNode* cur = head;
        while (cur) {
            cur->data->setColour(colour);
            cur = cur->next;
        }
    }
    
    explicit SceneGraph(std::vector<Shape*> source) {
        for (auto src : source) {
            addShape(src);
        }
    }
    
    virtual void translate(glm::mat4& translation) override {
        Shape::translate(translation);
        SceneGraphNode* cur = head;
        while (cur) {
            cur->data->translate(translation);
            cur = cur->next;
        }
    }
    
    void addShape(Shape* shape) {
        if (!head) {
            SceneGraphNode* node = new SceneGraphNode();
            node->data = shape;
            node->next = 0;
            head = node;
        }
        else {
            SceneGraphNode* tmp = head;
            while (tmp->next) {
                tmp = tmp->next;
            }
            SceneGraphNode* node = new SceneGraphNode();
            node->data = shape;
            node->next = 0;
            tmp->next = node;
        }
    }
    
    void addShapes(std::vector<Shape*> shapes) {
        for (auto shape : shapes) {
            addShape(shape);
        }
    }
    
    virtual void render(ShaderProgram shaderProgram) override {
        SceneGraphNode* tmp = head;
        float angle = glm::radians(1.0f);
        addRotationTransform(glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f)));
        while (tmp) {
            tmp->data->render(shaderProgram);
            tmp = tmp->next;
        }
        Shape::renderAABB(getAABB(), shaderProgram);
    }
    
    virtual  void addRotationTransform(glm::mat4&& transform) override {
        modellingTransform = modellingTransform * transform;
        SceneGraphNode* tmp = head;
        while (tmp) {
            tmp->data->addRotationTransform(std::move(transform));
            tmp = tmp->next;
        }
    }
    
    virtual void setModelingTransform(glm::mat4&& transform) override {
        SceneGraphNode* tmp = head;
        Shape::setModelingTransform(glm::mat4(transform));
        while (tmp) {
            tmp->data->setModelingTransform(glm::mat4(transform));
            tmp = tmp->next;
        }
    }
    
    virtual void updateModellingTransform(glm::mat4&& transform) override {
        Shape::updateModellingTransform(glm::mat4(transform));
    }
    
    void setPosition(glm::vec3 position) {
        Shape::translate(position);
    }
    
    virtual Shape* clone() override {
        return new SceneGraph(*this);
    }
    
    virtual std::vector<glm::vec3> getPositions() override {
        std::vector<glm::vec3> positions{};
        SceneGraphNode* cur = head;
        while (cur) {
            for (auto pos : cur->data->getPositions()) {
                positions.push_back(pos);
            }
            cur = cur->next;
        }
        return positions;
    }
    
    virtual std::vector<glm::vec3> getAABB() override {
        if (!head) {
            return std::vector<glm::vec3>({glm::vec3(0.0,0.0f,0.0)});
        }
        std::vector<glm::vec3> allPositions = getPositions();
        return Shape::computeAABB(allPositions);
    }
    
    virtual ~SceneGraph() {
        SceneGraphNode* tmp = head;
        dtor_helper_rec(tmp);
    }
    
    SceneGraph() {}
};


class SceneGraphBuilder : public ShapeBuilder {
public:
    SceneGraphBuilder() {
        shape = new SceneGraph();
    }
    
    SceneGraph* build() override {
        return (SceneGraph*)shape->clone();
    }

};

/*
 For now a scene is made up of shapes. It is responsible for their memory as well.
 
 TODO: This should just be in the renderer
 
 TODO: Expose only an iterator, also will need to couple shapes with a shader program.
 */
class Scene {
private:
    std::vector<Shape*> shapes;
    
public:

    std::vector<Shape*> get() {
        return shapes;
    }
    
    void addMesh(Shape* shape) {
        shapes.push_back(shape);
    }
    
    void removeMesh(Shape* shape) {
        shapes.erase(std::remove_if(shapes.begin(), shapes.end(), [&](auto s) {
            return shape == s;
        }));
        delete shape;
    }
    
    Scene() = default;
    
    ~Scene() {
        for (Shape* shape : shapes) {
            if (shape) {
                delete shape;
                shape = 0;
            }
        }
    }
};


#endif /* Scene_h */
