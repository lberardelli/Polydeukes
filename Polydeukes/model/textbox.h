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
#include <climits>

class ScrollBox;

class BigInt {
    friend ScrollBox;
private:
    unsigned int data[4]{};

    bool add_overflow(unsigned int a, int b)
    {
        if (b > 0 && a > UINT_MAX - b) return true;
        return false;
    }
public:
    
    void print() const {
        for (int i = 0; i < 4; ++i) {
            std::cout << std::setw(8) << std::setfill('0') << std::hex << data[i];
        }
        std::cout << std::endl;
    }
    
    void add(int input) {
        if (input > 0) {
            if (add_overflow(data[3], input)) {
                unsigned int remainder = UINT_MAX - data[3];
                data[3] = 0;
                data[3] = remainder;
                if (add_overflow(data[2], 1)) {
                    data[2] = 0;
                    if (add_overflow(data[1], 1)) {
                        data[1] = 0;
                        if (add_overflow(data[0], 1)) {
                            data[0] = 0;
                        } else {
                            data[0] += 1;
                        }
                    } else {
                        data[1] += 1;
                    }
                } else {
                    data[2] += 1;
                }
            } else {
                data[3] += input;
            }
        } else if (input < 0) {
            if (data[3] < ((unsigned int)-1 * input)) {
                input += data[3];
                data[3] = UINT_MAX;
                if (data[2] < ((unsigned int)(-1 * input))) {
                    data[2] = UINT_MAX;
                    if (data[1] < ((unsigned int)(-1 * input))) {
                        data[1] = UINT_MAX;
                        if (data[0] < ((unsigned int)(-1 * input))) {
                            data[0] = UINT_MAX;
                        } else {
                            data[0] = data[0] - 1;
                        }
                    } else {
                        data[1] = data[1] - 1;
                    }
                } else {
                    data[2] = data[2] - 1;
                }
            }
            data[3] = data[3] + input;
        }
    }
    
};

class ScrollBox : public Shape, public std::enable_shared_from_this<ScrollBox> {
private:
    std::shared_ptr<Glyph> characters[32];
    BigInt internalValue;
    std::shared_ptr<FontManager> fontManager;
    float width,height;
    ScrollBox(const ScrollBox& that) : Shape(that) {
        for (int i = 0; i < 32; ++i) {
            characters[i] = std::dynamic_pointer_cast<Glyph>(that.characters[i]->clone());
        }
    }
    
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        ScrollBox* box = static_cast<ScrollBox*>(glfwGetWindowUserPointer(window));
        if (action == GLFW_PRESS) {
            switch (key) {
                case GLFW_KEY_DOWN: {
                    box->internalValue.add(-1);
                    box->updateScrollBar();
                    break;
                }
                case GLFW_KEY_UP: {
                    box->internalValue.add(1);
                    box->updateScrollBar();
                }
                case GLFW_KEY_RIGHT: {
                    box->internalValue.print();
                }
            }
        }
    }
    
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        ScrollBox* box = static_cast<ScrollBox*>(glfwGetWindowUserPointer(window));
        if (yoffset > 0) {
            yoffset += 1;
        } else {
            yoffset -= 1;
        }
        box->internalValue.add((int)yoffset);
        box->updateScrollBar();
    }
    
public:
    
    ScrollBox(GLFWwindow* window, std::shared_ptr<FontManager> fontManager, float width, float height) : fontManager(fontManager), width(width), height(height) {
        glfwSetWindowUserPointer(window, this);
        //glfwSetScrollCallback(window, scroll_callback);
        glfwSetKeyCallback(window, keyCallback);
        for (int i = 0; i < 32; ++i) {
            characters[i] = fontManager->getFromUnicode('0');
            std::vector<glm::vec3> corners;
            corners.push_back(glm::vec3(-width, -height, 0));
            corners.push_back(glm::vec3(width, height, 0));
            auto emToWorldTransform = computeEmToWorldTransform(corners, characters[i], fontManager->unitsPerEm);
            //now need to transform to local position inside the scrollbox.
            glm::mat4 worldToFontScale = glm::scale(glm::mat4(1.f), glm::vec3(width/32.f, width/32.f, 1.f));
            glm::mat4 translateToCursorPosition = glm::translate(glm::mat4(1.0f), glm::vec3((float)(i) * width/32.f, 0.f,0.f));
            emToWorldTransform = translateToCursorPosition * modellingTransform * worldToFontScale * emToWorldTransform;
            characters[i]->setModelingTransform(emToWorldTransform);
        }
    }
    
    void updateScrollBar() {
        for (int i = 0; i < 4; ++i) {
            unsigned int blockData = internalValue.data[i];
            for (int j = 0; j < 8; ++j) {
                unsigned int charData = blockData;
                int charIndex = i * 8 + j;
                int shiftValue = j * 4;
                charData = charData << shiftValue;
                charData = charData >> 28;
                if (charData > 9) { //accomodation of ascii table
                    charData += 7;
                }
                characters[charIndex] = fontManager->getFromUnicode('0' + charData);
                std::vector<glm::vec3> corners;
                corners.push_back(glm::vec3(-width,-height, 0));
                corners.push_back(glm::vec3(width, height, 0));
                auto transform = computeEmToWorldTransform(corners, characters[charIndex], fontManager->unitsPerEm);
                glm::mat4 worldToFontScale = glm::scale(glm::mat4(1.f), glm::vec3(width/32.f, width/32.f, 1.f));
                glm::mat4 translateToCursorPosition = glm::translate(glm::mat4(1.0f), glm::vec3((float)(charIndex) * width/32.f, 0.f,0.f));
                transform = translateToCursorPosition * modellingTransform * worldToFontScale * transform;
                characters[charIndex]->setModelingTransform(transform);
            }
        }
    }
    
    virtual std::shared_ptr<Shape> clone() override {
        auto retval = std::shared_ptr<ScrollBox>(new ScrollBox(*this));
        retval->referenceToThis = retval;
        return retval;
    }
    
    virtual void render(ShaderProgram shaderProgram) override {
        for (int i = 0; i < 32; ++i) {
            characters[i]->render(shaderProgram);
        }
    }
    
    void initReferenceToThis() {
        referenceToThis = shared_from_this();
    }
    
    unsigned int* getInternalData() {
        return internalValue.data;
    }
};

class TextBox : public Shape, public std::enable_shared_from_this<TextBox> {
private:
    struct GlyphAndCursorPosition {
        std::shared_ptr<Glyph> fill;
        glm::vec3 wsCursorPosition;
        unsigned int asUnicode;
        GlyphAndCursorPosition(std::shared_ptr<Glyph> fill, glm::vec3 wsCursorPosition, unsigned int asUnicode) : fill(fill), wsCursorPosition(wsCursorPosition), asUnicode(asUnicode) {}
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
            GlyphAndCursorPosition newGacp = GlyphAndCursorPosition(fill, wsCursorPosition, gac.asUnicode);
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
        box->addCharacter(codepoint);
    }
    
    void addCharacter(unsigned int codepoint) {
        float x = width / ((float)manager->unitsPerEm);
        float y = height / ((float)manager->unitsPerEm);
        float s = std::min(x,y);
        float scaleX = 1.f;
        float scaleY = scaleX;
        auto fill = manager->getFromUnicode(codepoint);
        auto emToWorld = this->emToWorld(fill, s);
        auto worldToFont = this->worldToFont(emToWorld, scaleX, scaleY);
        fill->setModelingTransform(glm::translate(glm::mat4(1.f), glm::vec3(0.f,0.f,cursorPosition.z + .001)) * worldToFont * emToWorld);
        glm::vec4 deltaAdvance = glm::scale(glm::mat4(1.0f), glm::vec3(s, s, 1.f)) * glm::vec4(fill->advanceWidth, 0.f,0.f,1.f);
        glm::mat4 worldToFontScale = glm::scale(glm::mat4(1.f), glm::vec3(scaleX, scaleY, 1.f));
        gandcp.push_back(GlyphAndCursorPosition(fill, cursorPosition, codepoint));
        cursorPosition.x += (worldToFontScale * deltaAdvance).x;
        glm::vec3 pos = getPosition();
        float heightOfRow = 0.5f;
        if (cursorPosition.x >= pos.x + width/2.f) {
            cursorPosition.x = pos.x-width/2.f; cursorPosition.y -= heightOfRow;
            if (cursorPosition.y <= pos.y - height/2.f) {
                canvas->setModelingTransform(canvas->getModellingTransform() * glm::scale(glm::mat4(1.f), glm::vec3(1.f,1.5f, 1.f)));
            }
        }
        auto cTrans = glm::translate(glm::mat4(1.0f), glm::vec3(cursorPosition.x, cursorPosition.y-(heightOfRow/2.f), cursorPosition.z + .001));
        auto cScale = glm::scale(glm::mat4(1.0f), glm::vec3(.01f, heightOfRow, 1.f));
        cursor->setModelingTransform(cTrans * cScale);
    }
    
    void resetTextData() {
        cursorPosition = glm::vec3(canvas->getPosition().x - width/2.f, canvas->getPosition().y + height/2.f, canvas->getPosition().z);
        gandcp.clear();
    }
        
public:
    
    void setData(std::string data) {
        resetTextData();
        for (auto c : data) {
            addCharacter(c);
        }
    }
    
    void setData(std::vector<int> data) {
        resetTextData();
        for (auto c : data) {
            addCharacter(c);
        }
    }
    
    std::vector<int> getData() const {
        std::vector<int> ret{};
        for (auto g : gandcp) {
            ret.push_back(g.asUnicode);
        }
        return ret;
    }
    
    void initReferenceToThis() {
        referenceToThis = shared_from_this();
    }
    
    TextBox(GLFWwindow* window, std::shared_ptr<FontManager> manager, float width, float height, ShaderProgram* glyphShader) : manager(manager), width(width), height(height) {
        canvas = std::dynamic_pointer_cast<Square>(SquareBuilder().withColour(glm::vec3(0.f,0.f,0.f)).build());
        TextBox(window, manager, canvas, width, height, glyphShader);
    }
    
    TextBox(GLFWwindow* window, std::shared_ptr<FontManager> manager, std::shared_ptr<Square> canvas, float width, float height, ShaderProgram* glyphShader) : manager(manager), width(width), height(height), canvas(canvas) {
        cursor = std::dynamic_pointer_cast<Square>(SquareBuilder().withColour(glm::vec3(1.f,1.f,1.f)).build());
        this->canvas->setModelingTransform(glm::scale(glm::mat4(1.f), glm::vec3(width, height, 1.f)));
        cursorPosition = glm::vec3(-width/2.f, height/2.f, 0.f);
        glyphShaderProgram = glyphShader;
        float heightOfRow = 0.5f;
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
