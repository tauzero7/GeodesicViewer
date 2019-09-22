/**
 * @file    rendertext.h
 * @author  Thomas Mueller
 *
 * @brief  Render text in OpenGL.
 *
 * This file is part of GeodesicView.
 */
#ifndef RENDER_TEXT_H
#define RENDER_TEXT_H

#include <cstring>
#include <iostream>

#include <QColor>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>

#ifdef HAVE_FREETYPE

#define ALIGN_LEFT 0x0001
#define ALIGN_RIGHT 0x0002
#define ALIGN_HCENTER 0x0004
#define ALIGN_TOP 0x0020
#define ALIGN_BOTTOM 0x0040
#define ALIGN_VCENTER 0x0080

class RenderTextPimpl;

class RenderText
{
public:
    /**
     * @brief Standard c'tor.
     * @param fontFilename : filename of font
     * @param size : size of font
     */
    RenderText(const char* fontFilename, unsigned int size);

    /**
     * @brief Standard c'tor.
     * @param fontFilename : filename of font.
     * @param size : size of font.
     * @param color : text color.
     */
    RenderText(const char* fontFilename, unsigned int size, float* rgba);
    RenderText(const char* fontFilename, unsigned int size, float r, float g, float b, float a = 1.0f);
    ~RenderText();

    unsigned int GetFontSize();

    float GetTextWidth(const char* text);

    /**
     * @brief Set text color.
     * @param r : red component [0,1]
     * @param g : green component [0,1]
     * @param b : blue component [0,1]
     */
    void SetColor(float r, float g, float b, float a = 1.0f);
    void SetColor(float* rgba);

    void GetColor(float& r, float& g, float& b, float& a);

    void SetFontSize(unsigned int size);
    void SetFontFilename(const char* fontFilename);

    void render(const char* text, float posx, float posy, float sx = 1.0f, float sy = 1.0f);
    void renderCentered(const char* text, float posx, float posy, float sx = 1.0f, float sy = 1.0f);
    void printf(float x, float y, const char* fmt, ...);

    /**
     * @brief Print text at position (px,py).
     * @param px    Horizontal pixel coordinate from left.
     * @param py    Vertical pixel coordinate from bottom.
     * @param text
     * @param centered
     */
    void print(const int px, const int py, const char* text, bool centered = false);

    void Print(const int px, const int py, const char* text, int align = ALIGN_LEFT);

    void Print(const int px, const int py, int align, const char* fmt, ...);

    /**
     * @brief Set rotation angle.
     * @param angle [degree]
     */
    void SetRotation(const float angle);

    void SetWindowSize(int width, int height);

protected:
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

    bool init();
    void clear();

    QString getVertexShaderCode();
    QString getFragmentShaderCode();

    typedef struct Coord_t {
        float x;
        float y;
        float z;
        float w;
        Coord_t() { this->x = this->y = this->z = this->w = 0.0f; }
        Coord_t(float x, float y, float z, float w)
        {
            this->x = x;
            this->y = y;
            this->z = z;
            this->w = w;
        }
    } Coord;

private:
    std::string fontFilename_;
    unsigned int fontSize_;
    bool initialized_;

    float winWidth;
    float winHeight;
    float rotAngle;

    RenderTextPimpl* rtp;

    QColor color_;

    QOpenGLBuffer vbo;
    QOpenGLVertexArrayObject va;
    QOpenGLShaderProgram* shader;
};

#endif // HAVE_FREETYPE

#endif // RENDER_TEXT_H
