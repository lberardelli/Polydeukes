//
//  textbox.h
//  Polydeukes
//
//  Created by Lawrence Berardelli on 2025-03-23.
//

#ifndef textbox_h
#define textbox_h

#include "shape.h"
#include <glm.hpp>

class TextBox : public Shape, public std::enable_shared_from_this<TextBox> {
private:
    struct GlyphAndCursorPosition {
        std::shared_ptr<Glyph> fill;
        glm::vec3 wsCursorPosition;
        
        GlyphAndCursorPosition(std::shared_ptr<Glyph> fill, glm::vec3 wsCursorPosition) : fill(fill), wsCursorPosition(wsCursorPosition) {}
    };
    std::shared_ptr<Square> canvas;
    std::shared_ptr<Square> cursor;
    std::shared_ptr<FontManager> manager;
    glm::vec3 cursorPosition;
    std::deque<GlyphAndCursorPosition> gandcp;
    float width; float height;
    ShaderProgram* glyphShaderProgram;
    
    TextBox(const TextBox& that) : Shape(that) {
        canvas = std::static_pointer_cast<Square>(that.canvas->clone());
        cursor = std::static_pointer_cast<Square>(that.cursor->clone());
        manager = that.manager;
        cursorPosition = that.cursorPosition;
        for (auto gac : gandcp) {
            glm::vec3 wsCursorPosition = gac.wsCursorPosition;
            auto fill = std::static_pointer_cast<Glyph>(gac.fill->clone());
            GlyphAndCursorPosition newGacp = GlyphAndCursorPosition(fill, wsCursorPosition);
            gandcp.push_back(newGacp);
        }
        width = that.width; height = that.height; glyphShaderProgram = that.glyphShaderProgram;
    }
    
    glm::mat4 emToWorld(std::shared_ptr<Glyph> fill, float s) {
        glm::vec2 centre = glm::vec2((fill->getEmSpaceBoundingBox()[2]+fill->getEmSpaceBoundingBox()[0])/2.f, (fill->getEmSpaceBoundingBox()[3] + fill->getEmSpaceBoundingBox()[1])/2.f);
        float boxy;
        float boxx;
        float minboxxw = -width/2.f + ((fill->getEmSpaceBoundingBox()[0])/(float)manager->unitsPerEm * width);
        float minboxyw = -height/2.f + ((fill->getEmSpaceBoundingBox()[1])/(float)manager->unitsPerEm * height);
        glm::mat4 emToWorld = glm::scale(glm::mat4(1.0f), glm::vec3(s, s, 1.f)) * glm::translate(glm::mat4(1.0f), glm::vec3(-1.f * centre.x, -1.f * centre.y, 0.0f));
        float worldSpaceBoundingBox[4];
        for (int i = 0; i < 4; ++i) {
            if (i % 2 == 0) {
                worldSpaceBoundingBox[i] = (emToWorld * glm::vec4(fill->getEmSpaceBoundingBox()[i], 0.f,0.f,1.0f)).x;
            }
            else {
                worldSpaceBoundingBox[i] = (emToWorld * glm::vec4(0.f, fill->getEmSpaceBoundingBox()[i],0.f,1.0f)).y;
            }
        }
        boxx = -worldSpaceBoundingBox[0] + minboxxw;
        boxy = -worldSpaceBoundingBox[1] + minboxyw;
        
        return glm::translate(glm::mat4(1.0f), glm::vec3(boxx, boxy, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(s, s, 1.f)) * glm::translate(glm::mat4(1.0f), glm::vec3(-1.f * centre.x, -1.f * centre.y, 0.0f));
    }
    
    glm::mat4 worldToFont(glm::mat4& emToWorld, float scaleX, float scaleY) {
        glm::mat4 worldToFontScale = glm::scale(glm::mat4(1.f), glm::vec3(scaleX, scaleY, 1.f));
        float fontSpaceEmSquare[4];
        fontSpaceEmSquare[0] = (worldToFontScale * emToWorld * glm::vec4(0.f,0.f,0.f,1.f)).x;
        fontSpaceEmSquare[1] = (worldToFontScale * emToWorld * glm::vec4(0.f,0.f,0.f,1.f)).y;
        fontSpaceEmSquare[2] = (worldToFontScale * emToWorld * glm::vec4((float)(manager->unitsPerEm),0.f,0.f,1.f)).x;
        fontSpaceEmSquare[3] = (worldToFontScale * emToWorld * glm::vec4(0.f,(float)(manager->unitsPerEm),0.f,1.f)).y;
        //translate to cursor position
        glm::mat4 translateToCursorPosition = glm::translate(glm::mat4(1.0f), glm::vec3(cursorPosition.x - fontSpaceEmSquare[0], cursorPosition.y - fontSpaceEmSquare[3], 0.f));
        return translateToCursorPosition * worldToFontScale;
    }
    
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        TextBox* box = static_cast<TextBox*>(glfwGetWindowUserPointer(window));
        if (action == GLFW_PRESS) {
            switch (key) {
                case GLFW_KEY_BACKSPACE: {
                    if (box->gandcp.size() == 0) {
                        return;
                    }
                    auto deleted = box->gandcp.back();
                    box->gandcp.pop_back();
                    box->cursorPosition = deleted.wsCursorPosition;
                    float scaleX = .05f;
                    float heightOfRow = box->height * scaleX;
                    auto cTrans = glm::translate(glm::mat4(1.0f), glm::vec3(box->cursorPosition.x, box->cursorPosition.y-(heightOfRow/2.f), .001));
                    auto cScale = glm::scale(glm::mat4(1.0f), glm::vec3(.01, heightOfRow, 1.f));
                    box->cursor->setModelingTransform(cTrans * cScale);
                    break;
                }
                case GLFW_KEY_ENTER: {
                    float scaleX = .05f;
                    float heightOfRow = box->height * scaleX;
                    glm::vec pos = box->getPosition();
                    box->cursorPosition.x = pos.x-box->width/2.f; box->cursorPosition.y -= heightOfRow;
                    auto cTrans = glm::translate(glm::mat4(1.0f), glm::vec3(box->cursorPosition.x, box->cursorPosition.y-(heightOfRow/2.f), .001));
                    auto cScale = glm::scale(glm::mat4(1.0f), glm::vec3(.01, heightOfRow, 1.f));
                    box->cursor->setModelingTransform(cTrans * cScale);
                    break;
                }
            }
        }
    }
    
    static void characterCallback(GLFWwindow* window, unsigned int codepoint) {
        TextBox* box = static_cast<TextBox*>(glfwGetWindowUserPointer(window));
        float x = box->width / ((float)box->manager->unitsPerEm);
        float y = box->height / ((float)box->manager->unitsPerEm);
        float s = std::min(x,y);
        float scaleX = .05f;
        float scaleY = scaleX;
        auto fill = box->manager->getFromUnicode(codepoint);
        auto emToWorld = box->emToWorld(fill, s);
        auto worldToFont = box->worldToFont(emToWorld, scaleX, scaleY);
        fill->setModelingTransform(glm::translate(glm::mat4(1.f), glm::vec3(0.f,0.f,box->cursorPosition.z + .001)) * worldToFont * emToWorld);
        glm::vec4 deltaAdvance = glm::scale(glm::mat4(1.0f), glm::vec3(s, s, 1.f)) * glm::vec4(fill->advanceWidth, 0.f,0.f,1.f);
        glm::mat4 worldToFontScale = glm::scale(glm::mat4(1.f), glm::vec3(scaleX, scaleY, 1.f));
        box->gandcp.push_back(GlyphAndCursorPosition(fill, box->cursorPosition));
        box->cursorPosition.x += (worldToFontScale * deltaAdvance).x;
        glm::vec3 pos = box->getPosition();
        float heightOfRow = box->height * scaleX;
        if (box->cursorPosition.x >= pos.x + box->width/2.f) {
            box->cursorPosition.x = pos.x-box->width/2.f; box->cursorPosition.y -= heightOfRow;
            if (box->cursorPosition.y <= pos.y - box->height/2.f) {
                box->canvas->setModelingTransform(box->canvas->getModellingTransform() * glm::scale(glm::mat4(1.f), glm::vec3(1.f,1.5f, 1.f)));
            }
        }
        auto cTrans = glm::translate(glm::mat4(1.0f), glm::vec3(box->cursorPosition.x, box->cursorPosition.y-(heightOfRow/2.f), box->cursorPosition.z + .001));
        auto cScale = glm::scale(glm::mat4(1.0f), glm::vec3(.01f, heightOfRow, 1.f));
        box->cursor->setModelingTransform(cTrans * cScale);
    }
        
public:
    
    void initReferenceToThis() {
        referenceToThis = shared_from_this();
    }
    
    TextBox(GLFWwindow* window, std::shared_ptr<FontManager> manager, float width, float height, ShaderProgram* glyphShader) : manager(manager), width(width), height(height) {
        canvas = std::dynamic_pointer_cast<Square>(SquareBuilder().withColour(glm::vec3(0.f,0.f,0.f)).build());
        cursor = std::dynamic_pointer_cast<Square>(SquareBuilder().withColour(glm::vec3(1.f,1.f,1.f)).build());
        canvas->setModelingTransform(glm::scale(glm::mat4(1.f), glm::vec3(width, height, 1.f)));
        cursorPosition = glm::vec3(-width/2.f, height/2.f, 0.f);
        glyphShaderProgram = glyphShader;
        float scaleY = .05f;
        float heightOfRow = height * scaleY;
        auto cScale = glm::scale(glm::mat4(1.0f), glm::vec3(.01, heightOfRow, 1.f));
        auto cTrans = glm::translate(glm::mat4(1.0f), glm::vec3(cursorPosition.x, cursorPosition.y-(heightOfRow/2.f), 0.f));
        cursor->setModelingTransform(cTrans * cScale);
        glfwSetWindowUserPointer(window, this);
        glfwSetCharCallback(window, characterCallback);
        glfwSetKeyCallback(window, keyCallback);
        setOnClick([window](std::weak_ptr<Shape> thisBox) {
            MeshDragger::registerMousePositionCallback(window, thisBox);
        });
    }
    
    void render(ShaderProgram shaderProgram) override {
        canvas->render(shaderProgram);
        cursor->render(shaderProgram);
        for (auto& gandc : gandcp) {
            glyphShaderProgram->bind();
            gandc.fill->render(*glyphShaderProgram);
        }
    }
    
    std::shared_ptr<Shape> clone() override {
        auto retval = std::shared_ptr<TextBox>(new TextBox(*this));
        retval->referenceToThis = retval;
        return retval;
    }
    
    void updateModellingTransform(glm::mat4&& delta) override {
        canvas->setModelingTransform(glm::mat4(delta * canvas->getModellingTransform()));
        cursor->setModelingTransform(glm::mat4(delta * cursor->getModellingTransform()));
        cursorPosition = delta * glm::vec4(cursorPosition,1.f);
        for (auto& t : gandcp) {
            t.fill->setModelingTransform(delta * t.fill->getModellingTransform());
            t.wsCursorPosition = delta * glm::vec4(t.wsCursorPosition, 1.f);
        }
    }
    
    glm::vec3 getPosition() const override {
        glm::vec4 tmp = glm::vec4(0.0f,0.0f,0.0f,1.0f);
        tmp = canvas->getModellingTransform() * tmp;
        return glm::vec3(tmp.x,tmp.y,tmp.z);
    }
    
    void setModelingTransform(glm::mat4&& transform) override {
        canvas->setModelingTransform(transform);
        cursor->setModelingTransform(transform);
        cursorPosition = transform * glm::vec4(cursorPosition,1.f);
        for (auto& t : gandcp) {
            t.fill->setModelingTransform(transform);
            t.wsCursorPosition = transform * glm::vec4(t.wsCursorPosition, 1.f);
        }
    }
    
    void setModelingTransform(glm::mat4& transform) override {
        canvas->setModelingTransform(transform);
        cursor->setModelingTransform(transform);
        cursorPosition = transform * glm::vec4(cursorPosition,1.f);
        for (auto& t : gandcp) {
            t.fill->setModelingTransform(transform);
            t.wsCursorPosition = transform * glm::vec4(t.wsCursorPosition, 1.f);
        }
    }
    
    std::vector<glm::vec3> getAABB() override {
        return canvas->getAABB();
    }
};


#endif /* textbox_h */
