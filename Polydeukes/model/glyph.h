//
//  glyph.h
//  Polydeukes
//
//  Created by Lawrence Berardelli on 2025-01-22.
//

#ifndef glyph_h
#define glyph_h

#include "shape.h"
#include <vector>
#include <glm.hpp>
#include <glad/glad.h>
#include <thread>
#include <mutex>
#include "../view/screenheight.h"

unsigned int GRANULARITY = 100;

std::vector<glm::vec3> computeBezierCurve(std::vector<glm::vec3>& controlPoints) {
    std::array<glm::vec3, 4> currentCurve;
    std::vector<glm::vec3> positions{};
    for (int i = 0; i < controlPoints.size()-3; i+=3) {
        currentCurve[0] = controlPoints[i];
        currentCurve[1] = controlPoints[i+1];
        currentCurve[2] = controlPoints[i+2];
        currentCurve[3] = controlPoints[i+3];
        float parameterValue = 0.f;
        for (int j = 0; j < GRANULARITY; ++j) {
            glm::vec3 firstInterpolatedValue = currentCurve[1]*(parameterValue) + currentCurve[0] * (1.f-parameterValue);
            glm::vec3 secondInterpolatedValue = currentCurve[2]*(parameterValue) + currentCurve[1] * (1.f-parameterValue);
            glm::vec3 thirdInterpolatedValue = currentCurve[3]*(parameterValue) + currentCurve[2] * (1.f-parameterValue);
            glm::vec3 secondFirstIV = secondInterpolatedValue*(parameterValue) + firstInterpolatedValue*(1.f-parameterValue);
            glm::vec3 secondsecondIV = thirdInterpolatedValue*(parameterValue) + secondInterpolatedValue*(1.f-parameterValue);
            glm::vec3 thirdFirstIV = secondsecondIV * (parameterValue) + secondFirstIV * (1.f-parameterValue);
            positions.push_back(thirdFirstIV);
            parameterValue += (float)1.f/(float)GRANULARITY;
        }
    }
    return positions;
}


class Glyph : public Shape {
private:
    GLuint vao, vbo, ebo, sdfTexture;
    int numEdges{};
    bool bInitialized = false;
    float sdfData[128 * 128]{};
    
    Glyph(const Glyph& that) : Shape(that), numEdges(that.numEdges), vao(that.vao), vbo(that.vbo), ebo(that.ebo), sdfTexture(that.sdfTexture), bInitialized(that.bInitialized) {
        if (!that.bInitialized) {
            for (int i = 0; i < 128; ++i) {
                for (int j = 0; j < 128; ++j) {
                    sdfData[j * 128 + i] = that.sdfData[j*128 + i];;
                }
            }
        }
        for (int i = 0; i < 4; ++i) {
            boundingBox[i] = that.boundingBox[i];
        }
    }
    
    float computeSignedDistance(glm::vec2 p, std::vector<glm::vec4>& edges, int numEdges) {
        float minDist = std::numeric_limits<float>::max();
        bool inside = false;
        int windingNumber = 0;
        for (int i = 0; i < numEdges; ++i) {
            glm::vec2 a = glm::vec2(edges[i].x, edges[i].y);
            glm::vec2 b = glm::vec2(edges[i].z, edges[i].w);
            
            // Compute the distance from p to line segment (a, b)
            glm::vec2 ab = b - a;
            glm::vec2 ap = p - a;
            float t = glm::clamp(glm::dot(ap, ab) / glm::dot(ab, ab), 0.0f, 1.0f);
            glm::vec2 closest = a + t * ab;
            float dist = glm::length(closest - p);

            // Keep track of the minimum distance
            minDist = glm::min(minDist, dist);

            // Inside-outside test (winding rule)
            if (a.y <= p.y) {
                if (b.y > p.y && (b.x - a.x) * (p.y - a.y) > (p.x - a.x) * (b.y - a.y))
                    windingNumber++;
            } else {
                if (b.y <= p.y && (b.x - a.x) * (p.y - a.y) < (p.x - a.x) * (b.y - a.y))
                    windingNumber--;
            }
        }
        inside = (windingNumber != 0);
        // If inside, return positive distance; if outside, return negative
        return inside ? -minDist : minDist;
    }
    
    void init() {
        glGenTextures(1, &sdfTexture);
        glBindTexture(GL_TEXTURE_2D, sdfTexture);

        // Allocate storage for the SDF texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 128, 128, 0, GL_RED, GL_FLOAT, nullptr);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 128, 128, GL_RED, GL_FLOAT, sdfData);

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };
        
        float quadVertices[] = {
            boundingBox[0], boundingBox[1],
            boundingBox[2], boundingBox[1],
            boundingBox[2], boundingBox[3],
            boundingBox[0], boundingBox[3]
        };

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

public:
    
    float boundingBox[4];
    
    virtual void render(ShaderProgram shaderProgram) override {
        if (!bInitialized) {
            init();
            bInitialized = true;
        }
        shaderProgram.setVec2("resolution", glm::vec2(ScreenHeight::screen_width,ScreenHeight::screen_height));
        shaderProgram.setVec2("minBounds", glm::vec2(boundingBox[0],boundingBox[1]));
        shaderProgram.setVec2("maxBounds", glm::vec2(boundingBox[2],boundingBox[3]));
        shaderProgram.setMat4("model", modellingTransform);
        float threshold = 0.f;
        shaderProgram.setFloat("threshold", threshold);
        glBindTexture(GL_TEXTURE_2D, sdfTexture);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    
    virtual std::shared_ptr<Shape> clone() override {
        auto retval = std::shared_ptr<Glyph>(new Glyph(*this));
        retval->referenceToThis = retval;
        return retval;
    }
    
    Glyph(const std::vector<std::vector<glm::vec2>>& worldSpaceBezierPaths) {
        std::vector<glm::vec4> edges{};
        std::vector<glm::vec2> polygon{};
        for (auto path : worldSpaceBezierPaths) {
            for (int i = 0; i < path.size(); ++i) {
                int next = i + 1;
                if (i == path.size()-1) {
                    next = 0;
                }
                edges.push_back(glm::vec4(path[i].x, path[i].y, path[next].x, path[next].y));
                polygon.push_back(glm::vec2(path[i].x, path[i].y));
            }
        }
        numEdges = (int)edges.size();
        //compute the bounding box
        float minX = std::numeric_limits<float>::max();
        float minY = std::numeric_limits<float>::max();
        float maxX = -1 * std::numeric_limits<float>::max();
        float maxY = -1 * std::numeric_limits<float>::max();
        for (const auto& vertex : polygon) {
            minX = std::min(minX, vertex.x);
            minY = std::min(minY, vertex.y);
            maxX = std::max(maxX, vertex.x);
            maxY = std::max(maxY, vertex.y);
        }
        
        float quadVertices[] = {
            minX, minY,
            maxX, minY,
            maxX, maxY,
            minX, maxY
        };
        
        boundingBox[0] = minX; boundingBox[1] = minY; boundingBox [2] = maxX; boundingBox[3] = maxY;
        
        float cellSizeY = (maxY - minY) / 128.f;
        float cellSizeX = (maxX - minX) / 128.f;
        for (int i = 0; i < 128; ++i) {
            for (int j = 0; j < 128; ++j) {
                float worldX = minX + i * cellSizeX;
                float worldY = minY + j * cellSizeY;
                glm::vec2 gridPoint = glm::vec2(worldX, worldY);

                // Compute SDF value (distance to nearest glyph edge)
                float sdf = computeSignedDistance(gridPoint, edges, numEdges);

                sdfData[j * 128 + i] = sdf;  // Store in row-major order
            }
        }
    }
    
};

class FontLoader;

class FontManager {
    friend FontLoader;
private:
    struct GlyphWithIndex {
        std::shared_ptr<Shape> glyph{};
        int index;
        
        GlyphWithIndex(std::shared_ptr<Shape> glyph, int index) : glyph(glyph), index(index) {}
    };
    std::vector<GlyphWithIndex> theFont{};
    
    void put(std::shared_ptr<Shape> glyph, int index) {
        int eraseIndex = -1;
        for (int i = 0; i < theFont.size(); ++i) {
            if (theFont[i].index == index) {
                eraseIndex = i;
                break;
            }
        }
        if (eraseIndex >= 0) {
            theFont.erase(theFont.begin() + eraseIndex);
        }
        theFont.push_back(GlyphWithIndex(glyph, index));
    }
    
public:
    
    FontManager() {}
    
    std::shared_ptr<Shape> get(int index) {
        for (auto gi : theFont) {
            if (gi.index == index) {
                if (gi.glyph == nullptr) {
                    return nullptr;
                }
                return gi.glyph->clone();
            }
        }
        return nullptr;
    }
    
};


class FontLoader {
private:
    
    static void readlbpFontFile(std::string pathToGlyph, std::vector<std::vector<std::vector<glm::vec3>>>& bezierPaths) {
        std::ifstream ifs(pathToGlyph);
        if (ifs.is_open()) {
            std::string line{};
            std::vector<std::vector<glm::vec3>> path{};
            std::vector<glm::vec3> segment{};
            while (std::getline(ifs, line)) {
                if (line == "paths:") {
                    
                }
                else if (line == "\tpath:") {
                    if (!segment.empty()) {
                        path.push_back(segment);
                    }
                    if (!path.empty()) {
                        bezierPaths.push_back(path);
                    }
                    path.clear();
                    segment.clear();
                }
                else if (line == "\t\tsegment:") {
                    if (!segment.empty()) {
                        path.push_back(segment);
                    }
                    segment.clear();
                }
                else if (line.compare(0, 3, "\t\t\t") == 0) {
                    float f1,f2,f3;
                    std::sscanf(line.c_str(), "\t\t\t%f %f %f", &f1, &f2, &f3);
                    segment.push_back(glm::vec3(f1,f2,f3));
                }
            }
            if (!segment.empty()) {
                path.push_back(segment);
            }
            if (!path.empty()) {
                bezierPaths.push_back(path);
            }
        }
        else {
            std::cout << "Failed to open file!" << std::endl;
        }
    }
    
    static std::shared_ptr<Glyph> computeGlyphFromPaths(std::vector<std::vector<std::vector<glm::vec3>>>& bezierPaths, std::vector<std::vector<glm::vec2>>& worldSpacePaths) {
        for (auto path : bezierPaths) {
            std::vector<glm::vec2> worldSpacePath{};
            for (auto segment : path) {
                std::vector<glm::vec3> curve = computeBezierCurve(segment);
                for (auto position : curve) {
                    worldSpacePath.push_back(position);
                }
            }
            worldSpacePaths.push_back(worldSpacePath);
        }
        return std::shared_ptr<Glyph>(new Glyph(worldSpacePaths));
    }
    
public:
    
    static std::shared_ptr<FontManager> loadFont(const std::string& pathToFontDirectory, std::array<std::function<void(std::shared_ptr<Glyph>)>, 27>& callbacks) {
        std::shared_ptr<FontManager> manager = std::shared_ptr<FontManager>(new FontManager());
        for (int i = 0; i < 27; ++i) {
            std::thread([&, manager, i]() {
                std::vector<std::vector<std::vector<glm::vec3>>> paths{};
                std::vector<std::vector<glm::vec2>> worldSpacePaths{};
                std::string pathToGlyph = pathToFontDirectory + "/" + std::to_string(i);
                readlbpFontFile(pathToGlyph, paths);
                auto glyph = computeGlyphFromPaths(paths, worldSpacePaths);
                callbacks[i](glyph);
                manager->put(glyph, i);
            }).detach();
        }
        return manager;
    }
    
    static std::shared_ptr<Glyph> reloadGlyph(const std::string& pathToFontDirectory, const std::string& fontFile, std::shared_ptr<FontManager> manager) {
        std::vector<std::vector<std::vector<glm::vec3>>> paths{};
        std::vector<std::vector<glm::vec2>> worldSpacePaths{};
        std::string pathToGlyph = pathToFontDirectory + "/" + fontFile;
        readlbpFontFile(pathToGlyph, paths);
        auto glyph = computeGlyphFromPaths(paths, worldSpacePaths);
        manager->put(glyph, std::stoi(fontFile));
        return glyph;
    }
    
};


#endif /* glyph_h */
