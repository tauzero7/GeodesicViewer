/**
 * @file    rendertext.cpp
 * @author  Thomas Mueller
 *
 * This file is part of GeodesicView.
 */
#include "utils/rendertext.h"
#include "utils/mathutils.h"
#include <QMatrix4x4>
#include <stdint.h>

#ifdef HAVE_FREETYPE

#include "ft2build.h"
#include FT_FREETYPE_H

typedef struct char_info_t {
    float adv_x; //!< advance in x direction
    float adv_y; //!< advance in y direction
    float bitmap_width;
    float bitmap_height;
    float bitmap_left;
    float bitmap_top;
    float tx; //!< x offset of glyph in texture coordinates
} char_info;

typedef struct font_atlas_t {
    GLuint texID; //!< texture id
    float width; //!< width of texture
    float height; //!< height of texture
    char_info c[128]; //!< character information
} font_atlas;

class RenderTextPimpl
{
public:
    RenderTextPimpl();

    FT_Library ft_;
    FT_Face face_;
    FT_GlyphSlot g_;
    font_atlas atlas_;
};

RenderTextPimpl::RenderTextPimpl()
{
    atlas_.texID = 0;
}

RenderText::RenderText(const char* fontFilename, unsigned int size)
{
    fontFilename_ = std::string(fontFilename);
    fontSize_ = size;
    color_ = Qt::white;
    shader = nullptr;

    rtp = new RenderTextPimpl();
    initialized_ = init();
}

RenderText::RenderText(const char* fontFilename, unsigned int size, float* rgba)
{
    fontFilename_ = std::string(fontFilename);
    fontSize_ = size;
    color_ = QColor();
    color_.setRedF(static_cast<qreal>(rgba[0]));
    color_.setGreenF(static_cast<qreal>(rgba[1]));
    color_.setBlueF(static_cast<qreal>(rgba[2]));
    color_.setAlphaF(static_cast<qreal>(rgba[3]));
    shader = nullptr;

    rtp = new RenderTextPimpl();
    initialized_ = init();
}

RenderText::RenderText(const char* fontFilename, unsigned int size, float r, float g, float b, float a)
{
    fontFilename_ = std::string(fontFilename);
    fontSize_ = size;
    color_ = QColor();
    color_.setRedF(static_cast<qreal>(r));
    color_.setGreenF(static_cast<qreal>(g));
    color_.setBlueF(static_cast<qreal>(b));
    color_.setAlphaF(static_cast<qreal>(a));
    shader = nullptr;

    rtp = new RenderTextPimpl();
    initialized_ = init();
}

RenderText::~RenderText()
{
    clear();
}

unsigned int RenderText::GetFontSize()
{
    return fontSize_;
}

float RenderText::GetTextWidth(const char* text)
{
    float sx = 1.0f;

    const uint8_t* p;
    float width = 0.0;
    const char* t = text;
    for (p = reinterpret_cast<const uint8_t*>(t); *p; p++) {
        float w = rtp->atlas_.c[*p].adv_x * sx;
        if (w < 1e-10f)
            continue;

        width += w;
    }
    return width;
}

void RenderText::SetColor(float r, float g, float b, float a)
{
    color_ = QColor();
    color_.setRedF(static_cast<qreal>(r));
    color_.setGreenF(static_cast<qreal>(g));
    color_.setBlueF(static_cast<qreal>(b));
    color_.setAlphaF(static_cast<qreal>(a));
}

void RenderText::SetColor(float* rgba)
{
    if (rgba != nullptr) {
        color_ = QColor();
        color_.setRedF(static_cast<qreal>(rgba[0]));
        color_.setGreenF(static_cast<qreal>(rgba[1]));
        color_.setBlueF(static_cast<qreal>(rgba[2]));
        color_.setAlphaF(static_cast<qreal>(rgba[3]));
    }
}

void RenderText::GetColor(float& r, float& g, float& b, float& a)
{
    r = static_cast<float>(color_.redF());
    g = static_cast<float>(color_.greenF());
    b = static_cast<float>(color_.blueF());
    a = static_cast<float>(color_.alphaF());
}

void RenderText::SetFontSize(unsigned int size)
{
    clear();
    fontSize_ = std::max(4U, size);
    rtp = new RenderTextPimpl();
    initialized_ = init();
}

void RenderText::SetFontFilename(const char* fontFilename)
{
    clear();
    fontFilename_ = std::string(fontFilename);
    rtp = new RenderTextPimpl();
    initialized_ = init();
}

void RenderText::SetWindowSize(int width, int height)
{
    winWidth = static_cast<float>(width);
    winHeight = static_cast<float>(height);
}

void RenderText::renderCentered(const char* text, float posx, float posy, float sx, float sy)
{
    float tw = GetTextWidth(text);
    render(text, posx - 0.5f * tw, posy, sx, sy);
}

void RenderText::render(const char* text, float posx, float posy, float sx, float sy)
{
    if (!initialized_) {
        fprintf(stderr, "RenderText not initialized!\n");
        init();
        return;
    }

    if (shader == nullptr) {
        return;
    }

    GLboolean isBlendingEnabled = glIsEnabled(GL_BLEND);

    // store old blend function
    GLint blend_src_rgb, blend_src_alpha, blend_dst_rgb, blend_dst_alpha;
    glGetIntegerv(GL_BLEND_SRC_RGB, &blend_src_rgb);
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &blend_src_alpha);
    glGetIntegerv(GL_BLEND_DST_RGB, &blend_dst_rgb);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &blend_dst_alpha);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_DEPTH_TEST);

    QMatrix4x4 projMX;
    projMX.ortho(0.0f, winWidth, 0.0f, winHeight, -1.0f, 1.0f);

    QMatrix4x4 viewMX;
    viewMX.setToIdentity();
    viewMX.translate(posx, posy, 0.0f);
    // rotAngle in degree !!
    viewMX.rotate(rotAngle, 0.0f, 0.0f, 1.0f);

    float x = 0.0f;
    float y = 0.0f;

    Coord* coords_ = new Coord[6 * strlen(text)];

    const uint8_t* p;
    const char* t = text;
    int c = 0;
    for (p = reinterpret_cast<const uint8_t*>(t); *p; p++) {
        float x2 = x + rtp->atlas_.c[*p].bitmap_left * sx;
        float y2 = -y - rtp->atlas_.c[*p].bitmap_top * sy;
        float w = rtp->atlas_.c[*p].bitmap_width * sx;
        float h = rtp->atlas_.c[*p].bitmap_height * sy;

        x += rtp->atlas_.c[*p].adv_x * sx;
        y += rtp->atlas_.c[*p].adv_y * sy;

        // if (!w || !h)    continue;

        coords_[c++] = Coord(x2, -y2, rtp->atlas_.c[*p].tx, 0);
        coords_[c++] = Coord(x2 + w, -y2, rtp->atlas_.c[*p].tx + rtp->atlas_.c[*p].bitmap_width / rtp->atlas_.width, 0);
        coords_[c++] = Coord(x2, -y2 - h, rtp->atlas_.c[*p].tx, rtp->atlas_.c[*p].bitmap_height / rtp->atlas_.height);
        coords_[c++] = Coord(x2 + w, -y2, rtp->atlas_.c[*p].tx + rtp->atlas_.c[*p].bitmap_width / rtp->atlas_.width, 0);
        coords_[c++] = Coord(x2, -y2 - h, rtp->atlas_.c[*p].tx, rtp->atlas_.c[*p].bitmap_height / rtp->atlas_.height);
        coords_[c++] = Coord(x2 + w, -y2 - h, rtp->atlas_.c[*p].tx + rtp->atlas_.c[*p].bitmap_width / rtp->atlas_.width,
            rtp->atlas_.c[*p].bitmap_height / rtp->atlas_.height);
    }

    shader->bind();

    //glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, rtp->atlas_.texID);

    shader->setUniformValue("tex", 0);
    shader->setUniformValue("color", color_);
    shader->setUniformValue("projMX", projMX);
    shader->setUniformValue("viewMX", viewMX);

    vbo.create();
    vbo.bind();
    vbo.allocate(&coords_[0], 4 * c * static_cast<int>(sizeof(float)));

    va.bind();
    shader->enableAttributeArray(0);
    shader->setAttributeBuffer(0, GL_FLOAT, 0, 4);

    glDrawArrays(GL_TRIANGLES, 0, c);
    vbo.release();
    va.release();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    shader->release();

    if (!isBlendingEnabled) {
        glDisable(GL_BLEND);
    }
    else {
        // recover old blend function
        glBlendFunc(static_cast<GLenum>(blend_src_rgb), static_cast<GLenum>(blend_dst_rgb));
        //  glBlendFuncSeparate(blend_src_rgb, blend_dst_rgb, blend_src_alpha, blend_dst_alpha);
    }
}

void RenderText::printf(float x, float y, const char* fmt, ...)
{
    va_list ap;
    char text[1024];
    if (fmt == nullptr)
        return;

    va_start(ap, fmt);
#ifdef _WIN32
    vsprintf_s(text, fmt, ap);
#else
    vsprintf(text, fmt, ap);
#endif
    va_end(ap);
    render(text, x, y);
}

void RenderText::print(const int px, const int py, const char* text, bool centered)
{
    if (centered) {
        renderCentered(text, static_cast<float>(px), static_cast<float>(py));
    }
    else {
        render(text, static_cast<float>(px), static_cast<float>(py));
    }
}

void RenderText::Print(const int px, const int py, const char* text, int align)
{
    float x = static_cast<float>(px);
    float y = static_cast<float>(py);
    float tw = GetTextWidth(text);

    if (align & ALIGN_RIGHT) {
        x -= tw;
    }
    else if (align & ALIGN_HCENTER) {
        x -= 0.5f * tw;
    }

    if (align & ALIGN_TOP) {
        y -= fontSize_;
    }
    else if (align & ALIGN_VCENTER) {
        y -= 0.5f * fontSize_;
    }

    render(text, x, y);
}

void RenderText::Print(const int px, const int py, int align, const char* fmt, ...)
{
    va_list ap;
    char text[1024];
    if (fmt == nullptr)
        return;

    va_start(ap, fmt);
#ifdef _WIN32
    vsprintf_s(text, fmt, ap);
#else
    vsprintf(text, fmt, ap);
#endif
    va_end(ap);

    float x = static_cast<float>(px);
    float y = static_cast<float>(py);
    float tw = GetTextWidth(text);

    if (align & ALIGN_RIGHT) {
        x -= tw;
    }
    else if (align & ALIGN_HCENTER) {
        x -= 0.5f * tw;
    }

    if (align & ALIGN_TOP) {
        y -= fontSize_;
    }
    else if (align & ALIGN_VCENTER) {
        y -= 0.5f * fontSize_;
    }

    render(text, x, y);
}

void RenderText::SetRotation(const float angle)
{
    rotAngle = angle;
}

bool RenderText::init()
{
    fprintf(stdout, "Initialize RenderText using '%s' ...\n", fontFilename_.c_str());
    if (FT_Init_FreeType(&rtp->ft_)) {
        fprintf(stderr, "@ %s (%d): Could not init freetype library.\n", __FILE__, __LINE__);
        return false;
    }

    if (FT_New_Face(rtp->ft_, fontFilename_.c_str(), 0, &rtp->face_)) {
        char msg[256];
#ifdef _WIN32
        sprintf_s(msg, "Could not open font %s.", fontFilename_.c_str());
#else
        sprintf(msg, "Could not open font %s.", fontFilename_.c_str());
#endif
        fprintf(stderr, "@ %s (%d): %s\n", __FILE__, __LINE__, msg);
        return false;
    }

    FT_Set_Pixel_Sizes(rtp->face_, 0, fontSize_);
    rtp->g_ = rtp->face_->glyph;

    int minw = 0;
    int minh = 0;
    memset(rtp->atlas_.c, 0, sizeof(rtp->atlas_.c));

    for (int i = 32; i < 128; i++) {
        if (FT_Load_Char(rtp->face_, i, FT_LOAD_RENDER)) {
            char msg[1024];
#ifdef _WIN32
            sprintf_s(msg, "Loading character %c failed.", i);
#else
            sprintf(msg, "Loading character %c failed.", i);
#endif
            fprintf(stderr, "@ %s (%d): %s\n", __FILE__, __LINE__, msg);
            continue;
        }
        minw += rtp->g_->bitmap.width + 1;
        minh = std::max(minh, static_cast<int>(rtp->g_->bitmap.rows));
    }

    rtp->atlas_.width = static_cast<float>(minw);
    rtp->atlas_.height = static_cast<float>(minh);

    glGenTextures(1, &rtp->atlas_.texID);

    glBindTexture(GL_TEXTURE_2D, rtp->atlas_.texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // CLAMP_TO_BORDER?
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, minw, minh, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    int offset = 0;
    for (int i = 32; i < 128; i++) {
        if (FT_Load_Char(rtp->face_, i, FT_LOAD_RENDER)) {
            char msg[256];
#ifdef _WIN32
            sprintf_s(msg, "Loading character %c failed.", i);
#else
            sprintf(msg, "Loading character %c failed.", i);
#endif
            fprintf(stderr, "@ %s (%d): %s\n", __FILE__, __LINE__, msg);
            continue;
        }
        glTexSubImage2D(GL_TEXTURE_2D, 0, offset, 0, rtp->g_->bitmap.width, rtp->g_->bitmap.rows, GL_RED,
            GL_UNSIGNED_BYTE, rtp->g_->bitmap.buffer);

        rtp->atlas_.c[i].adv_x = static_cast<float>(rtp->g_->advance.x >> 6);
        rtp->atlas_.c[i].adv_y = static_cast<float>(rtp->g_->advance.y >> 6);
        rtp->atlas_.c[i].bitmap_width = static_cast<float>(rtp->g_->bitmap.width);
        rtp->atlas_.c[i].bitmap_height = static_cast<float>(rtp->g_->bitmap.rows);
        rtp->atlas_.c[i].bitmap_left = static_cast<float>(rtp->g_->bitmap_left);
        rtp->atlas_.c[i].bitmap_top = static_cast<float>(rtp->g_->bitmap_top);
        rtp->atlas_.c[i].tx = offset / rtp->atlas_.width;
        offset += rtp->g_->bitmap.width + 1;
    }

    // glGenerateMipmap(GL_TEXTURE_2D);
    fprintf(stderr, "Generated a %d x %d (%d kb) texture atlas\n", minw, minh, minw * minh / 1024);

    shader = new QOpenGLShaderProgram();
    shader->addShaderFromSourceCode(QOpenGLShader::Vertex, getVertexShaderCode());
    shader->addShaderFromSourceCode(QOpenGLShader::Fragment, getFragmentShaderCode());
    shader->link();
    std::cerr << shader->log().toStdString() << std::endl;

    glBindTexture(GL_TEXTURE_2D, 0);

    vbo.create();
    vbo.bind();
    vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    va.create();
    vbo.release();
    return true;
}

void RenderText::clear()
{
    if (rtp != nullptr && rtp->atlas_.texID > 0) {
        glDeleteTextures(1, &rtp->atlas_.texID);
    }

    if (rtp != nullptr) {
        delete[] rtp;
        rtp = nullptr;
    }

    vbo.destroy();
    va.destroy();
}

QString RenderText::getVertexShaderCode()
{
    QString vert;
    vert += "#version 330\n\n";
    vert += "uniform mat4 projMX;\n";
    vert += "uniform mat4 viewMX;\n";
    vert += "layout(location = 0) in  vec4 coord;\n";
    vert += "out vec2 texpos;\n";
    vert += "void main(void) {\n";
    vert += "    gl_Position = projMX * viewMX * vec4(coord.xy, 0, 1);\n";
    vert += "    texpos = coord.zw;\n";
    vert += "}\n";
    return vert;
}

QString RenderText::getFragmentShaderCode()
{
    QString frag;
    frag += "#version 330\n\n";
    frag += "uniform sampler2D tex;\n";
    frag += "uniform vec4 color;\n";
    frag += "in vec2 texpos;\n";
    frag += "layout(location = 0) out vec4 out_frag_color;\n";
    frag += "void main(void) {\n";
    frag += "  out_frag_color = vec4(1, 1, 1, texture(tex, texpos).r) * color;\n";
    // frag += "  out_frag_color = vec4(1,1,0,1);\n";
    frag += "}\n";
    return frag;
}

#endif // HAVE_FREETYPE
