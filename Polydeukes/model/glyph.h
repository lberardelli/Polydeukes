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
#include "ttfinterpreter.h"
#include "spline.h"
#include "sphere.h"

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
    
    std::vector<std::shared_ptr<SplineCurve>> contours{};
    std::vector<std::shared_ptr<Shape>> reifiedControlPoints{};
    std::vector<std::vector<glm::vec3>> controlPoints{};
    
    void setModelingTransform(glm::mat4&& transform) override {
        Shape::setModelingTransform(transform);
        for (auto contour : contours) {
            contour->setModelingTransform(glm::mat4(transform));
        }
        for (auto controlPoint : reifiedControlPoints) {
            controlPoint->updateModellingTransform(std::move(transform));
        }
    }
    
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
    
    Glyph(const std::vector<std::vector<glm::vec2>>& worldSpaceBezierPaths, std::vector<std::vector<glm::vec3>> controlPoints) {
        this->controlPoints = controlPoints;
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
    
    struct MissingPoint {
        int index;
        int contour;
        Point point;
    };
    
    static std::vector<MissingPoint> fillMissingPoints(std::vector<Contour>& contours) {
        //these are quadratic bezier curves so off curve points have multiplicity 2.
        //if two points in a row are off curve, take the centre point as an on curve point.
        //if two points in a row are on curve, take the centre point as an off curve point.
        std::vector<MissingPoint> missingPoints{};
        int j = 0;
        for (auto contour : contours) {
            std::vector<Point> pts = contour.points;
            for (int i = 1; i < contour.points.size() + 1; ++i) {
                i = i % pts.size();
                int prev = i - 1;
                if (i == 0) {
                    prev = pts.size() - 1;
                }
                if (pts[prev].onCurve) {
                    if (pts[i].onCurve) {
                        glm::vec2 position = glm::vec2((pts[i].xCoord + pts[prev].xCoord)/2, (pts[i].yCoord + pts[prev].yCoord)/2);
                        Point p; p.onCurve = false; p.xCoord = position.x; p.yCoord = position.y;
                        MissingPoint mp; mp.index = i; mp.point = p; mp.contour = j;
                        missingPoints.push_back(mp);
                    }
                } else {
                    if (!pts[i].onCurve) {
                        glm::vec2 position = glm::vec2((pts[i].xCoord + pts[prev].xCoord)/2, (pts[i].yCoord + pts[prev].yCoord)/2);
                        Point p; p.onCurve = true; p.xCoord = position.x; p.yCoord = position.y;
                        MissingPoint mp; mp.index = i; mp.point = p; mp.contour = j;
                        missingPoints.push_back(mp);
                    }
                }
                if (i == 0) {
                    break;
                }
            }
            ++j;
        }
        return missingPoints;
    }
    
public:
    
    static std::shared_ptr<Glyph> computeGlyphFromTTFont(TTFont& font, std::vector<glm::vec3> corners , int j) {
        TTFGlyph glyph = font.glyphs[j];
        TTFGlyph absGlyph;
        float s = std::min(
            (corners[1].x - corners[0].x) / ((float)font.unitsPerEm),
            (corners[1].y - corners[0].y) / ((float)font.unitsPerEm)
        );
        glm::vec2 centre = glm::vec2((glyph.boundingBox[2]+glyph.boundingBox[0])/2.f, (glyph.boundingBox[3] + glyph.boundingBox[1])/2.f);
        glm::mat4 emToWorld = glm::scale(glm::mat4(1.0f), glm::vec3(s, s, 1.f)) * glm::translate(glm::mat4(1.0f), glm::vec3(-1.f * centre.x, -1.f * centre.y, 0.0f));
        glm::vec2 prevLocation = glm::vec2(0,0);
        std::vector<Contour> absContours{};
        std::vector<std::vector<glm::vec3>> controlPoints{};
        for (auto contour : font.glyphs[j].contours) {
            std::vector<Point> absPoints{};
            for (int i = 0; i < contour.points.size(); ++i) {
                Point point = contour.points[i];
                glm::vec2 currentLocation = glm::vec2(point.xCoord + prevLocation.x, point.yCoord + prevLocation.y);
                Point absPoint; absPoint.onCurve = point.onCurve; absPoint.xCoord = currentLocation.x; absPoint.yCoord = currentLocation.y;
                absPoints.push_back(absPoint);
                prevLocation = currentLocation;
            }
            Contour absContour; absContour.points = absPoints;
            absContours.push_back(absContour);
        }
        std::vector<MissingPoint> missingPoints = fillMissingPoints(absContours);
        std::vector<Point> contour{};
        std::vector<std::shared_ptr<SplineCurve>> splines{};
        std::vector<std::vector<glm::vec2>> worldSpaceBezierPaths{};
        for (int i = 0; i < absContours.size(); ++i) {
            std::vector<glm::vec3> contourControlPoints{};
            for (int k = 0; k < absContours[i].points.size(); ++k) {
                for (int l = 0; l < missingPoints.size(); ++l) {
                    if (missingPoints[l].index == k && missingPoints[l].contour == i) {
                        contour.push_back(missingPoints[l].point);
                        break;
                    }
                }
                contour.push_back(absContours[i].points[k]);
            }
            if (!contour[0].onCurve) {
                if (!contour[1].onCurve) {
                    throw std::exception();
                }
                for (int k = 1; k < contour.size()-2; k += 2) {
                    glm::vec3 first = emToWorld * glm::vec4(contour[k].xCoord, contour[k].yCoord, 0.f, 1.0f);
                    glm::vec3 second = emToWorld * glm::vec4(contour[k+1].xCoord, contour[k+1].yCoord, 0.f, 1.0f);
                    glm::vec3 third = emToWorld * glm::vec4(contour[k+1].xCoord, contour[k+1].yCoord, 0.f, 1.0f);
                    glm::vec3 fourth = emToWorld * glm::vec4(contour[k+2].xCoord, contour[k+2].yCoord, 0.f, 1.0f);
                    contourControlPoints.push_back(first);
                    contourControlPoints.push_back(second);
                    contourControlPoints.push_back(third);
                    contourControlPoints.push_back(fourth);
                }
                glm::vec3 first = emToWorld * glm::vec4(contour.back().xCoord, contour.back().yCoord, 0.f, 1.f);
                glm::vec3 second = emToWorld * glm::vec4(contour[0].xCoord, contour[0].yCoord, 0.f,1.f);
                glm::vec3 third = emToWorld * glm::vec4(contour[0].xCoord, contour[0].yCoord, 0.f,1.f);
                glm::vec3 fourth = emToWorld * glm::vec4(contour[1].xCoord, contour[1].yCoord, 0.f,1.f);
                contourControlPoints.push_back(first);
                contourControlPoints.push_back(second);
                contourControlPoints.push_back(third);
                contourControlPoints.push_back(fourth);
            } else {
                if (contour[1].onCurve) {
                    throw std::exception();
                }
                for (int k = 0; k < contour.size()-2; k += 2) {
                    glm::vec3 first = emToWorld * glm::vec4(contour[k].xCoord, contour[k].yCoord, 0.f, 1.0f);
                    glm::vec3 second = emToWorld * glm::vec4(contour[k+1].xCoord, contour[k+1].yCoord, 0.f, 1.0f);
                    glm::vec3 third = emToWorld * glm::vec4(contour[k+1].xCoord, contour[k+1].yCoord, 0.f, 1.0f);
                    glm::vec3 fourth = emToWorld * glm::vec4(contour[k+2].xCoord, contour[k+2].yCoord, 0.f, 1.0f);
                    contourControlPoints.push_back(first);
                    contourControlPoints.push_back(second);
                    contourControlPoints.push_back(third);
                    contourControlPoints.push_back(fourth);
                }
                glm::vec3 first = emToWorld * glm::vec4(contour[contour.size()-2].xCoord, contour[contour.size()-2].yCoord, 0.f, 1.f);
                glm::vec3 second = emToWorld * glm::vec4(contour.back().xCoord, contour.back().yCoord, 0.f,1.f);
                glm::vec3 third = emToWorld * glm::vec4(contour.back().xCoord, contour.back().yCoord, 0.f,1.f);
                glm::vec3 fourth = emToWorld * glm::vec4(contour[0].xCoord, contour[0].yCoord, 0.f,1.f);
                contourControlPoints.push_back(first);
                contourControlPoints.push_back(second);
                contourControlPoints.push_back(third);
                contourControlPoints.push_back(fourth);
            }
            std::vector<glm::vec3> tmp = computeBezierCurve(contourControlPoints);
            std::vector<glm::vec2> tmp2{};
            for (auto e : tmp) {
                tmp2.push_back(e);
            }
            worldSpaceBezierPaths.push_back(tmp2);
            controlPoints.push_back(contourControlPoints);
            contour.clear();
        }
        auto fill = std::shared_ptr<Glyph>(new Glyph(worldSpaceBezierPaths, controlPoints));
        return fill;
    }
    
    static std::shared_ptr<FontManager> loadFont(TTFont& font, std::vector<std::function<void(std::shared_ptr<Glyph>)>>& callbacks, std::vector<glm::vec3> corners) {
        std::shared_ptr<FontManager> manager = std::shared_ptr<FontManager>(new FontManager());
        for (int i = 0; i < callbacks.size(); ++i) {
            std::thread([&, manager, i, corners]() {
                auto glyph = computeGlyphFromTTFont(font, corners, i);
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
        return nullptr;
    }
    
};


#endif /* glyph_h */
