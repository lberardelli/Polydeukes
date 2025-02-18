//
//  ttfinterpreter.h
//  Polydeukes
//
//  Created by Lawrence Berardelli on 2025-02-10.
//

#ifndef ttfinterpreter_h
#define ttfinterpreter_h

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

unsigned int readuint32(std::vector<char>& buffer, int startIndex) {
    unsigned int retval = 0;
    for (int i = startIndex; i < startIndex + 4; ++i) {
        int pow = 255 * std::pow(16, (2*(3-(i-startIndex))));
        retval |= ((buffer[i] << (8 * (3-(i - startIndex)))) & pow);
    }
    return retval;
}

short readFword(std::vector<char>& buffer, int startIndex) {
    short retval = 0;
    for (int i = startIndex; i < startIndex + 2; ++i) {
        int pow = 255 * std::pow(16, (2*(1-(i-startIndex))));
        retval |= ((buffer[i] << (8 * (1-(i - startIndex)))) & pow);
    }
    return retval;
}

unsigned short readUShort(std::vector<char>& buffer, int startIndex) {
    unsigned short retval = 0;
    for (int i = startIndex; i < startIndex + 2; ++i) {
        int pow = 255 * std::pow(16, (2*(1-(i-startIndex))));
        retval |= ((buffer[i] << (8 * (1-(i - startIndex)))) & pow);
    }
    return retval;
}

short readShort(std::vector<char>& buffer, int startIndex) {
    return readFword(buffer, startIndex);
}

unsigned char readUByte(std::vector<char>& buffer, int startIndex) {
    unsigned char retval = buffer[startIndex] & 0xFF;
    return retval;
}

void printCoolShit(int j, std::vector<char>& buffer, std::string string) {
    unsigned int offset = 0;
    unsigned int length = 0;
    offset = (buffer[20 + j * 16] << 24) & 0xFF000000 | (buffer[21 + j * 16] << 16) & 0xFF0000 | (buffer[22 +j * 16] << 8) & 0xFF00 | buffer[23 + j*16] & 0xFF;
    length = (buffer[24 + j * 16] << 24) & 0xFF000000 | (buffer[25 + j * 16] << 16) & 0xFF0000 | (buffer[26 +j * 16] << 8) & 0xFF00 | buffer[27 + j*16] & 0xFF;
    std::cout << "Location of " << string << " table spec: " << 12 + j * 16 << std::endl;
    std::cout << "Offset of " << string << " table: " << offset << std::endl;
    std::cout << "length of " << string << " table: " << length << std::endl;
}


void test(char value) {
    std::cout << "val: " << (value << 24) << std::endl;
}

struct PointInfo {
    bool onCurve;
    bool xShort;
    bool yShort;
    bool xSame;
    bool ySame;
};

struct Point {
    bool onCurve;
    int xCoord;
    int yCoord;
};

struct Contour {
    std::vector<Point> points{};
};

struct TTFGlyph {
    std::vector<Contour> contours{};
    int index;
    int boundingBox[4];
};

struct TTFGlyphAndTransform {
    unsigned int glyphIndex;
    glm::mat4 transform;
};

struct TTFCompoundGlyph {
    int boundingBox[4];
    int index;
    std::vector<TTFGlyphAndTransform> gats{};
};

struct TTFont {
    std::vector<TTFGlyph> glyphs{};
    std::vector<TTFCompoundGlyph> compoundGlyphs{};
    unsigned short unitsPerEm;
    
    int getNGlyphs() const {
        return glyphs.size() + compoundGlyphs.size();
    }
    
    //this works because we are putting all simple glyphs in ignoring gaps for compound glyphs.
    bool isCompound(int i) const {
        return i >= glyphs.size();
    }
    
    int insertionIndexToGlyphIndex(int i) const {
        if (isCompound(i)) {
            return compoundGlyphs[i-glyphs.size()].index;
        }
        return glyphs[i].index;
    }
    
    int glyphIndexToInsertionIndex(int i) const {
        for (int j = 0; j < glyphs.size(); ++j) {
            if (glyphs[j].index == i) {
                return j;
            }
        }
        for (int j = 0; j < compoundGlyphs.size(); ++j) {
            if (compoundGlyphs[j].index == i) {
                return j + glyphs.size();
            }
        }
        return -1;
    }
    
    TTFGlyph getGlyph(int i) {
        if (isCompound(i)) {
            throw std::exception();
        }
        return glyphs[i];
    }
    
    TTFCompoundGlyph getCompoundGlyph(int i) {
        if (!isCompound(i)) {
            throw std::exception();
        }
        return compoundGlyphs[i-glyphs.size()];
    }
    
    int* getEmSpaceBoundingBox(int i) {
        if (isCompound(i)) {
            i -= glyphs.size();
            return compoundGlyphs[i].boundingBox;
        }
        return glyphs[i].boundingBox;
    }
};

TTFont interpret() {
    std::ifstream file("/Users/lawrenceberardelli/Downloads/open-sans/OpenSans-Bold.ttf", std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file. Code: " << file.rdstate() << " (" << strerror(errno) << ")" << std::endl;
        TTFont font;
        return font;
    }

    // Get the length of the file
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read the file into a vector of bytes
    std::vector<char> buffer(fileSize);
    file.read(buffer.data(), fileSize);

    // Check if the read was successful
    unsigned int glyphTableOffset = 0;
    unsigned int headTableOffset = 0;
    unsigned int locaTableOffset = 0;
    unsigned int maxpTableOffset = 0;
    if (file) {
        std::cout << "File read successfully!" << std::endl;
        // You can now access the raw binary data in the buffer
        int nTables = (buffer[4] << 8) | (buffer[5] & 0xFF);
        for (int j = 0; j < nTables; ++j) {
            char firstTableTag[5];
            for (int i = 0; i < 4; ++i) {
                firstTableTag[i] = buffer[12 + j * 16 + i];
            }
            firstTableTag[4] = '\0';
            if (strncmp(firstTableTag, "head", 4) == 0) {
                headTableOffset = readuint32(buffer, j * 16 + 20);
            }
            if (strncmp(firstTableTag, "loca", 4) == 0) {
                locaTableOffset = readuint32(buffer, j * 16 + 20);
            }
            if (strncmp(firstTableTag, "glyf", 4) == 0) {
                glyphTableOffset = readuint32(buffer, j*16 + 20);
            }
            if (strncmp(firstTableTag, "maxp", 4) == 0) {
                maxpTableOffset = readuint32(buffer, j * 16 + 20);
            }
        }
        std::cout << headTableOffset << " " << locaTableOffset << " " << glyphTableOffset << std::endl;
        unsigned int pointer = headTableOffset;
        unsigned short unitsPerEm = readUShort(buffer, pointer + 18);
        short offsetLength = readShort(buffer, pointer + 50);
        bool bShortGlyphOffsets = (offsetLength == (short)(0));
        std::cout << "units per em: " << unitsPerEm << std::endl;
        std::cout << "offset length value: " << offsetLength << std::endl;
        pointer = maxpTableOffset;
        unsigned short nGlyphs = readUShort(buffer, pointer + 4);
        std::cout << "nGlyphs: " << nGlyphs << std::endl;
        std::cout << "maxPoints: " << readUShort(buffer, pointer + 6) << std::endl;
        std::cout << "maxContours: " << readUShort(buffer, pointer + 8) << std::endl;
        pointer = locaTableOffset;
        std::vector<unsigned int> glyphOffsets{};
        if (bShortGlyphOffsets) {
            for (int i = 0; i <= nGlyphs; ++i) {
                unsigned short offset = readUShort(buffer, pointer);
                pointer+=2;
                glyphOffsets.push_back(offset * 2);
            }
        } else {
            for (int i = 0; i <= nGlyphs; ++i) {
                unsigned int offset = readuint32(buffer, pointer);
                pointer += 4;
                glyphOffsets.push_back(offset);
            }
        }
        std::vector<TTFGlyph> glyphs{};
        std::vector<TTFCompoundGlyph> compoundGlyphs{};
        for (int k = 0; k < nGlyphs; ++k) {
            if (glyphOffsets[k] == glyphOffsets[k+1]) {
                std::cout << "Found an empty glyph" << std::endl;
                glyphs.push_back(TTFGlyph());
                continue;
            }
            pointer = glyphTableOffset + glyphOffsets[k];
            short nContours = readShort(buffer, pointer); pointer += 2;
            if (nContours < 0) {
                TTFCompoundGlyph cg;
                cg.index = k;
                for (int i = 1; i < 5; ++i) {
                    short s = readShort(buffer, pointer);
                    cg.boundingBox[i-1] = s;
                    pointer += 2;
                }
                std::cout << "Found a compound glyph at " << k << " gonna skip for now!" << std::endl;
                bool more_components = true;
                while (more_components) {
                    unsigned short flags = readUShort(buffer, pointer); pointer += 2;
                    unsigned short index = readUShort(buffer, pointer); pointer += 2;
                    bool arg_1_2_are_words = flags & 0x1;
                    bool args_are_x_y_values = flags & 0x2;
                    bool round_xy_to_grid = flags & 0x4;
                    bool scale = flags & 0x8;
                    more_components = flags & 0x20;
                    bool x_and_y_scale = flags & 0x40;
                    bool two_by_two_transform = flags & 0x80;
                    bool instructions = flags & 0x100;
                    bool use_metrix = flags & 0x200;
                    bool overlap = flags & 0x400;
                    int e = 0, f = 0;
                    if (arg_1_2_are_words) {
                        if (args_are_x_y_values) {
                            e = readShort(buffer,pointer); pointer += 2;
                            f = readShort(buffer,pointer); pointer += 2;
                        } else {
                            unsigned short parentGlyphPoint = readUShort(buffer,pointer); pointer += 2;
                            unsigned short targetGlyphPoint = readUShort(buffer,pointer); pointer += 2;
                        }
                    } else {
                        if (args_are_x_y_values) {
                            e = static_cast<signed char>(readUByte(buffer, pointer)); pointer += 1;
                            f = static_cast<signed char>(readUByte(buffer, pointer)); pointer += 1;
                        } else {
                            unsigned short parentGlyphPoint = readUShort(buffer,pointer); pointer +=1;
                            unsigned short targetGlyphPoint = readUShort(buffer,pointer); pointer +=1;
                        }
                    }
                    float a = 1.f; float b = 0.f; float c = 0.f; float d = 1.f;
                    if (scale) {
                        a = readShort(buffer,pointer); pointer += 2;
                        d = a;
                    } else if (x_and_y_scale) {
                        a = readShort(buffer,pointer); pointer +=2;
                        d = readShort(buffer,pointer); pointer += 2;
                    } else if (two_by_two_transform) {
                        a = readShort(buffer,pointer); pointer += 2;
                        b = readShort(buffer,pointer); pointer += 2;
                        c = readShort(buffer,pointer); pointer += 2;
                        d = readShort(buffer,pointer); pointer += 2;
                    }
                    float m = std::max(std::abs(a), std::abs(b)); float n = std::max(std::abs(c), std::abs(d));
                    if (std::abs(std::abs(a)-std::abs(c)) <= (33.f/65536.f)) {
                        m = 2 * m;
                    }
                    if (std::abs(std::abs(b)-std::abs(d)) <= (33.f/65536.f)) {
                        n = 2 * n;
                    }
                    glm::mat4 transform = glm::mat4(a,c,0.f,0.f,b,d,0.f,0.f,0.f,0.f,0.f,0.f,e/m,f/n,0.f,1.f);
                    TTFGlyphAndTransform gat; gat.glyphIndex = index; gat.transform = transform;
                    cg.gats.push_back(gat);
                }
                compoundGlyphs.push_back(cg);
            }
            else {
                TTFGlyph glyph;
                glyph.index = k;
                for (int i = 1; i < 5; ++i) {
                    short s = readShort(buffer, pointer);
                    glyph.boundingBox[i-1] = s;
                    pointer += 2;
                }
                std::vector<unsigned short> contourEndpoints{};
                for (int i = 5; i < 5 + nContours; ++i) {
                    unsigned short s = readUShort(buffer, pointer);
                    pointer += 2;
                    contourEndpoints.push_back(s);
                }
                unsigned short instructionLength = readUShort(buffer, pointer);
                //skip instructions
                pointer += 2 + instructionLength;
                std::vector<unsigned char> flags{};
                unsigned int endpoint = contourEndpoints[contourEndpoints.size()-1] + 1;
                int nPointsAccountedFor = 0;
                std::vector<PointInfo> pointsInfo{};
                for (unsigned int i = pointer; i < pointer + endpoint; ++i) {
                    PointInfo pointInfo;
                    const uint8_t ON_CURVE = 1 << 0;
                    const uint8_t X_SHORT = 1 << 1;
                    const uint8_t Y_SHORT = 1 << 2;
                    const uint8_t REPEAT = 1 << 3;
                    const uint8_t X_SAME_OR_POSITIVE = 1 << 4;
                    const uint8_t Y_SAME_OR_POSITIVE = 1 << 5;
                    const uint8_t RESERVED_BITS = 0b11000000; // Bits 6-7 should be zero
                    unsigned char flag = readUByte(buffer, i);
                    pointInfo.onCurve = (flag & ON_CURVE);
                    pointInfo.xShort = (flag & X_SHORT);
                    pointInfo.yShort = (flag & Y_SHORT);
                    unsigned char n = 0;
                    if (flag & REPEAT) {
                        n = readUByte(buffer, i + 1);
                        nPointsAccountedFor += n;
                        i += 1; endpoint += (1 - (unsigned int)n);
                    }
                    pointInfo.xSame = (flag & X_SAME_OR_POSITIVE);
                    pointInfo.ySame = (flag & Y_SAME_OR_POSITIVE);
                    if (flag & RESERVED_BITS) {
                        std::cout << " - WARNING: Reserved bits should be zero!" << std::endl;
                    }
                    pointsInfo.push_back(pointInfo);
                    for (int j = 0; j < (int)n; ++j) {
                        pointsInfo.push_back(pointInfo);
                    }
                    ++nPointsAccountedFor;
                }
                pointer += endpoint;
                std::vector<Point> points{};
                for (int i = 0; i < pointsInfo.size(); ++i) {
                    Point point;
                    point.onCurve = pointsInfo[i].onCurve;
                    int xCoord = 0;
                    if (pointsInfo[i].xShort) {
                        xCoord = readUByte(buffer, pointer);
                        if (!pointsInfo[i].xSame) {
                            xCoord *= -1;
                        }
                        ++pointer;
                    } else {
                        if (pointsInfo[i].xSame) {
                        }
                        else {
                            xCoord = readShort(buffer, pointer);
                            pointer += 2;
                        }
                    }
                    point.xCoord = xCoord;
                    points.push_back(point);
                }
                for (int i = 0; i < pointsInfo.size(); ++i) {
                    int yCoord = 0;
                    if (pointsInfo[i].yShort) {
                        yCoord = readUByte(buffer, pointer);
                        if (!pointsInfo[i].ySame) {
                            yCoord *= -1;
                        }
                        ++pointer;
                    } else {
                        if (pointsInfo[i].ySame) {
                        }
                        else {
                            yCoord = readShort(buffer, pointer);
                            pointer += 2;
                        }
                    }
                    points[i].yCoord = yCoord;
                }
                std::vector<Contour> contours{};
                int prevEndpoint = 0;
                for (auto endpoint : contourEndpoints) {
                    std::vector<Point> cp{points.cbegin() + prevEndpoint, points.cbegin() + endpoint + 1};
                    Contour contour; contour.points = cp;
                    contours.push_back(contour);
                    prevEndpoint = endpoint + 1;
                }
                glyph.contours = contours;
                glyphs.push_back(glyph);
            }
        }
        TTFont font; font.glyphs = glyphs; font.compoundGlyphs = compoundGlyphs; font.unitsPerEm = unitsPerEm;
        return font;
    } else {
        std::cerr << "Error reading file!" << std::endl;
    }
    TTFont font;
    file.close();
    return font;
}


#endif /* ttfinterpreter_h */
