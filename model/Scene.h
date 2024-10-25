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
#include <sstream>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/string_cast.hpp>
#include <string>

#include "shape.h"
#include "sphere.h"
#include "vector.h"
#include "axies.h"
#include "square.h"

class SceneListNode {
public:
    std::shared_ptr<Shape> data;
    SceneListNode* next = 0;
    
    SceneListNode(SceneListNode& that) {
        data = that.data->clone();
        if (that.next) {
            next = new SceneListNode(*that.next);
        }
    }
    
    SceneListNode() = default;
    
};

class Scene;


//TODO: Sort out the mess of local offset and AABB. Whether we want each item of the scenelist to be clickable. etc.
class SceneList : public Shape {
    friend Scene;
private:
    SceneListNode* head = 0;
    void dtor_helper_rec(SceneListNode* tmp) {
        if (tmp->next) {
            dtor_helper_rec(tmp->next);
        }
        delete tmp;
    }
    
public:
    
    SceneList(const SceneList& that) : Shape(that) {
        if (that.head) {
            this->head = new SceneListNode(*that.head);
        }
    }
    
    void setColour(glm::vec3 colour) override {
        SceneListNode* cur = head;
        while (cur) {
            cur->data->setColour(colour);
            cur = cur->next;
        }
    }
    
    explicit SceneList(std::vector<std::shared_ptr<Shape>>&& source) {
        for (auto&& src : source) {
            addShape(std::move(src));
        }
    }
    
    virtual void translate(glm::mat4& translation) override {
        Shape::translate(translation);
        SceneListNode* cur = head;
        while (cur) {
            cur->data->translate(translation);
            cur = cur->next;
        }
    }
    
    void addShape(std::shared_ptr<Shape> shape) {
        if (!head) {
            SceneListNode* node = new SceneListNode();
            node->data = std::move(shape);
            node->next = 0;
            head = node;
        }
        else {
            SceneListNode* tmp = head;
            while (tmp->next) {
                tmp = tmp->next;
            }
            SceneListNode* node = new SceneListNode();
            node->data = std::move(shape);
            node->next = 0;
            tmp->next = node;
        }
    }
    
    void addShapes(std::vector<std::shared_ptr<Shape>>&& shapes) {
        for (auto&& shape : shapes) {
            addShape(std::move(shape));
        }
    }
    
    virtual void render(ShaderProgram shaderProgram) override {
        SceneListNode* tmp = head;
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
        SceneListNode* tmp = head;
        while (tmp) {
            tmp->data->addRotationTransform(std::move(transform));
            tmp = tmp->next;
        }
    }
    
    virtual void setModelingTransform(glm::mat4&& transform) override {
        SceneListNode* tmp = head;
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
    
    virtual std::shared_ptr<Shape> clone() override {
        auto retval = std::make_shared<SceneList>(*this);
        retval->referenceToThis = retval;
        return retval;
    }
    
    virtual std::vector<glm::vec3> getPositions() override {
        std::vector<glm::vec3> positions{};
        SceneListNode* cur = head;
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
    
    virtual ~SceneList() {
        if (head == 0) {
            return;
        }
        SceneListNode* tmp = head;
        dtor_helper_rec(tmp);
    }
    
    SceneList() {}
};


class SceneListBuilder : public ShapeBuilder {
public:
    SceneListBuilder() {
        shape = std::make_unique<SceneList>();
    }
    
    std::shared_ptr<Shape> build() override {
        return shape->clone();
    }

};

class SceneGraph;

class SceneGraphNode {
    friend SceneGraph;
private:
    std::vector<SceneGraphNode*> children{};
    std::vector<glm::vec3> localOffsets{};
    std::vector<glm::vec3> rotations{};
    std::string name{};
    std::vector<std::string> channels{};
    std::shared_ptr<Shape> data;
    std::shared_ptr<Shape> interGeometry{};
    SceneGraphNode* parent{};
    std::string rotationOrder{};
    std::string translationOrder{};
    
    void setFrameOffsetData(glm::vec3 offset) {
        localOffsets.push_back(offset);
    }
    
    void setFrameRotationData(glm::vec3 rotation) {
        rotations.push_back(rotation);
    }
    
    void duplicateOffsetData() {
        localOffsets.push_back(localOffsets[0]);
    }
    
    void duplicateRotationData() {
        if (rotations.empty()) {
            rotations.push_back(glm::vec3(0.f,0.f,0.f));
        }
        else {
            rotations.push_back(rotations[0]);
        }
    }
    
    SceneGraphNode(SceneGraphNode& that) : localOffsets(that.localOffsets), rotations(that.rotations), name(that.name), channels(that.channels), parent(that.parent), rotationOrder(that.rotationOrder), translationOrder(that.translationOrder) 
    {
        data = data->clone();
        interGeometry = interGeometry->clone();
        for (auto child : that.children) {
            children.push_back(new SceneGraphNode(*child));
        }
    }
    
public:
    SceneGraphNode(std::ifstream& sourceFileStream, std::string name, SceneGraphNode* parent = 0) : name(name), parent(parent) {
        std::string line{};
        data = SphereBuilder::getInstance()->build();
        interGeometry = CubeBuilder().build();
        while (std::getline(sourceFileStream, line)) {
            std::string delim = " ";
            std::string first = line.substr(0, line.find(delim));
            if (line.find("JOINT") != std::string::npos) {
                std::string name = line.substr(line.find("JOINT") + 5, line.length());
                children.push_back(new SceneGraphNode(sourceFileStream, name, this));
            }
            else if (line.find("End") != std::string::npos) {
                children.push_back(new SceneGraphNode(sourceFileStream, "End Site", this));
            }
            else if (line.find("OFFSET") != std::string::npos) {
                std::string seconds = line.substr(line.find("OFFSET") + 7, line.length());
                std::stringstream second(seconds);
                std::string x;
                char delim;
                if (line.find("\t") != std::string::npos) {
                    delim = '\t';
                }
                else {
                    delim = ' ';
                }
                std::getline(second, x, delim);
                glm::vec3 tmp{};
                tmp.x = std::stof(x);
                std::string y;
                std::getline(second, y, delim);
                tmp.y = std::stof(y);
                std::string z;
                std::getline(second, z);
                tmp.z = std::stof(z);
                localOffsets.push_back(tmp);
                rotations.push_back(glm::vec3(0.0f,0.0f,0.0f));
            }
            else if (line.find("CHANNELS") != std::string::npos) {
                std::string seconds = line.substr(line.find("CHANNELS") + 9, line.length());
                std::stringstream second(seconds);
                std::string channels;
                std::getline(second, channels, ' ');
                int nChannels = std::stoi(channels);
                for (int i = 0; i < nChannels; ++i) {
                    std::string channel;
                    std::getline(second, channel, ' ');
                    if (channel.find("Xposition") != std::string::npos) {
                        translationOrder.push_back('x');
                    }
                    else if (channel.find("Yposition") != std::string::npos) {
                        translationOrder.push_back('y');
                    }
                    else if (channel.find("Zposition") != std::string::npos) {
                        translationOrder.push_back('z');
                    }
                    else if (channel.find("Xrotation") != std::string::npos) {
                        this->rotationOrder.push_back('x');
                    }
                    else if (channel.find("Yrotation") != std::string::npos) {
                        this->rotationOrder.push_back('y');
                    }
                    else if (channel.find("Zrotation") != std::string::npos) {
                        this->rotationOrder.push_back('z');
                    }
                    this->channels.push_back(channel);
                }
            }
            else if (line.find("}") != std::string::npos) {
                return;
            }
        }
    }
    
    void render(ShaderProgram program, glm::mat4 accumulatedTranslation, int frameNumber) {
        if (name.find("End Site") != std::string::npos) {
            return;
        }
        glm::mat4 rot{};
        if (rotationOrder == "xyz") {
            glm::mat4 zrot = glm::rotate(glm::mat4(1.0f), glm::radians(rotations[frameNumber].z), glm::vec3(0.0f, 0.0f, 1.0f));
            glm::mat4 yrot = glm::rotate(glm::mat4(1.0f), glm::radians(rotations[frameNumber].y), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 xrot = glm::rotate(glm::mat4(1.0f), glm::radians(rotations[frameNumber].x), glm::vec3(1.0f, 0.0f, 0.0f));
            rot = xrot * yrot * zrot;
        }
        else if (rotationOrder == "xzy") {
            glm::mat4 yrot = glm::rotate(glm::mat4(1.0f), glm::radians(rotations[frameNumber].z), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 zrot = glm::rotate(glm::mat4(1.0f), glm::radians(rotations[frameNumber].y), glm::vec3(0.0f, 0.0f, 1.0f));
            glm::mat4 xrot = glm::rotate(glm::mat4(1.0f), glm::radians(rotations[frameNumber].x), glm::vec3(1.0f, 0.0f, 0.0f));
            rot = xrot * zrot * yrot;
        }
        else if (rotationOrder == "yxz") {
            glm::mat4 zrot = glm::rotate(glm::mat4(1.0f), glm::radians(rotations[frameNumber].z), glm::vec3(0.0f, 0.0f, 1.0f));
            glm::mat4 xrot = glm::rotate(glm::mat4(1.0f), glm::radians(rotations[frameNumber].y), glm::vec3(1.0f, 0.0f, 0.0f));
            glm::mat4 yrot = glm::rotate(glm::mat4(1.0f), glm::radians(rotations[frameNumber].x), glm::vec3(0.0f, 1.0f, 0.0f));
            rot = yrot * xrot * zrot;
        }
        else if (rotationOrder == "yzx") {
            glm::mat4 xrot = glm::rotate(glm::mat4(1.0f), glm::radians(rotations[frameNumber].z), glm::vec3(1.0f, 0.0f, 0.0f));
            glm::mat4 zrot = glm::rotate(glm::mat4(1.0f), glm::radians(rotations[frameNumber].y), glm::vec3(0.0f, 0.0f, 1.0f));
            glm::mat4 yrot = glm::rotate(glm::mat4(1.0f), glm::radians(rotations[frameNumber].x), glm::vec3(0.0f, 1.0f, 0.0f));
            rot = yrot * zrot * xrot;
        }
        else if (rotationOrder == "zxy") {
            glm::mat4 yrot = glm::rotate(glm::mat4(1.0f), glm::radians(rotations[frameNumber].z), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 xrot = glm::rotate(glm::mat4(1.0f), glm::radians(rotations[frameNumber].y), glm::vec3(1.0f, 0.0f, 0.0f));
            glm::mat4 zrot = glm::rotate(glm::mat4(1.0f), glm::radians(rotations[frameNumber].x), glm::vec3(0.0f, 0.0f, 1.0f));
            rot = zrot * xrot * yrot;
        }
        else if (rotationOrder == "zyx") {
            glm::mat4 xrot = glm::rotate(glm::mat4(1.0f), glm::radians(rotations[frameNumber].z), glm::vec3(1.0f, 0.0f, 0.0f));
            glm::mat4 yrot = glm::rotate(glm::mat4(1.0f), glm::radians(rotations[frameNumber].y), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 zrot = glm::rotate(glm::mat4(1.0f), glm::radians(rotations[frameNumber].x), glm::vec3(0.0f, 0.0f, 1.0f));
            rot = zrot * yrot * xrot;
        }
        data->setModelingTransform(accumulatedTranslation * glm::translate(glm::mat4(1.0f), localOffsets[frameNumber]) * rot * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f/5.f)));
        if (parent) {
            interGeometry->setModelingTransform(vector::scaleGeometryBetweenTwoPointsTransformation(data->getPosition(), parent->data->getPosition()));
            interGeometry->render(program);
        }
        data->render(program);
        for (auto child : children) {
            child->render(program, accumulatedTranslation * glm::translate(glm::mat4(1.0f), localOffsets[frameNumber]) * rot, frameNumber);
        }
    }
};

class SceneGraph : public Shape {
private:
    SceneGraphNode* head = 0;
    int fps;
    int nFrames;
    std::vector<float> frameData{};
    int nChannels{};
    int currentFrame{};
    
private:
    
    SceneGraph(SceneGraph& that) : fps(that.fps), nFrames(that.nFrames), nChannels(that.nChannels), currentFrame(that.currentFrame) {
        frameData = that.frameData;
    }
    
    
    int countChannels(SceneGraphNode* cur) {
        int n{};
        for (auto child : cur->children) {
            n += countChannels(child);
        }
        return n + (int)cur->channels.size();
    }
    
    void setPerFrameData(SceneGraphNode* tmp, int& offset) {
        int curChannelCount = (int)tmp->channels.size();
        if (curChannelCount == 6) {
            for (int i = 0; i < nFrames; ++i) {
                glm::vec3 offsetData = glm::vec3(frameData[offset + i * nChannels], frameData[offset + 1 + i * nChannels], frameData[offset + 2 + i * nChannels]);
                tmp->setFrameOffsetData(offsetData);
                glm::vec3 rotationData = glm::vec3(frameData[offset + 3 + i * nChannels], frameData[offset + 4 + i * nChannels], frameData[offset + 5 + i * nChannels]);
                tmp->setFrameRotationData(rotationData);
            }
        }
        else if (curChannelCount == 3) {
            for (int i = 0; i < nFrames; ++i) {
                glm::vec3 rotationData = glm::vec3(frameData[offset + i * nChannels], frameData[offset + 1 + i * nChannels], frameData[offset + 2 + i * nChannels]);
                tmp->setFrameRotationData(rotationData);
                tmp->duplicateOffsetData();
            }
        }
        else {
            tmp->duplicateOffsetData();
            tmp->duplicateRotationData();
        }
        offset += curChannelCount;
        for (auto child : tmp->children) {
            setPerFrameData(child, offset);
        }
    }
    
    
    void dtor_rec_helper(SceneGraphNode* cur) {
        if (cur == 0) {
            return;
        }
        for (auto child : cur->children) {
            dtor_rec_helper(child);
        }
        delete cur;
    }
    
public:
    SceneGraph(std::string& sourceFile) {
        std::ifstream bvhFile(sourceFile);
        std::string line{};
        if (bvhFile.is_open()) {
            while (std::getline(bvhFile, line)) {
                std::cout << line << "\n";
                std::string delim = " ";
                std::string first = line.substr(0, line.find(delim));
                if (line.find("ROOT") != std::string::npos) {
                    int index = line.find("ROOT") + 4;
                    std::string name = line.substr(index, line.length());
                    head = new SceneGraphNode(bvhFile, name);
                }
                else if (line.find("MOTION") != std::string::npos) {
                    while (std::getline(bvhFile, line)) {
                        if (line.find("Frames:") != std::string::npos) {
                            std::stringstream ss(line);
                            std::string frames;
                            char delim;
                            if (line.find("\t") != std::string::npos) {
                                delim = '\t';
                            }
                            else {
                                delim = ' ';
                            }
                            std::getline(ss, frames, delim);
                            std::getline(ss, frames);
                            nFrames = std::stoi(frames);
                        }
                        else if (line.find("Frame") != std::string::npos) {
                            std::string spf = line.substr(12, line.length());
                            float spff = std::stof(spf);
                            fps = (int)(1.0f/spff);
                        }
                        else {
                            std::string data{};
                            char delim;
                            if (line.find("\t") != std::string::npos) {
                                delim = '\t';
                            }
                            else {
                                delim = ' ';
                            }
                            std::stringstream ss(line);
                            while (std::getline(ss, data, delim)) {
                                if (data != "\r" && data != "") {
                                    frameData.push_back(std::stof(data));
                                }
                            }
                        }
                    }
                    
                }
            }
            bvhFile.close();
            nChannels = countChannels(head);
            int offset{};
            setPerFrameData(head, offset);
        }
        else {
            std::cout << "Unable to open file\n";
        }
    }
    
    virtual ~SceneGraph() {
        dtor_rec_helper(head);
    }
    
    virtual void render(ShaderProgram program) override {
        ++currentFrame;
        glm::mat4 defaultTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(0.f,0.f,0.f));
        head->render(program, defaultTranslation, currentFrame);
        if (currentFrame % (nFrames) == 0) {
            currentFrame = 1;
        }
    }
    
    virtual std::shared_ptr<Shape> clone() override {
        auto retval = std::shared_ptr<SceneGraph>(new SceneGraph(*this));
        retval->referenceToThis = retval;
        return retval;
    }
};

/*
 For now a scene is made up of shapes. It is responsible for their memory as well.
 
 TODO: This should just be in the renderer
 
 TODO: Expose only an iterator, also will need to couple shapes with a shader program.
 */
class Scene {
private:
    std::vector<std::shared_ptr<Shape>> shapes;
    
public:

    std::vector<std::shared_ptr<Shape>> get() {
        return shapes;
    }
    
    void addMesh(std::shared_ptr<Shape> shape) {
        shapes.push_back(shape);
    }
    
    void removeMesh(std::shared_ptr<Shape> shape) {
        shapes.erase(std::remove_if(shapes.begin(), shapes.end(), [&](auto& s) {
                    return shape.get() == s.get();
                }), shapes.end());
    }
    
    Scene() = default;
    
};


#endif /* Scene_h */
