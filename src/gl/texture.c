#include <texture.h>

GLenum convertPixels(const GLvoid *data, GLvoid **out,
                            GLsizei width, GLsizei height,
                            GLenum format, GLenum type) {
    unsigned int w;
    int m0, m1, m2, m3;
    GLenum newFormat = format;
    switch (format) {
        case GL_BGR:
            w = 3;
            m0 = 2; m1 = 1; m2 = 0;
            newFormat = GL_RGB;
            break;
        case GL_BGRA:
            w = 4;
            m0 = 2; m1 = 1; m2 = 0; m3 = 3;
            newFormat = GL_RGBA;
            break;
        default:
            printf("unsupported pixel format: %i\n", format);
            *out = (GLvoid *)data;
            return format;
    }

#define convert(constant, type)\
    case constant:\
        {\
            int size = width * height * w;\
            type *src = (type *)data;\
            type *dst = (type *)malloc(sizeof(type) * size);\
            for (int i = 0; i < size; i += w) {\
                dst[i]   = src[i+m0];\
                dst[i+1] = src[i+m1];\
                dst[i+2] = src[i+m2];\
                if (width == 4) dst[i+3] = src[i+m3];\
            }\
            *out = (GLvoid *)dst;\
        }\
        break;

    switch (type) {
        convert(GL_UNSIGNED_BYTE, GLubyte);
        convert(GL_FLOAT, GLfloat);
        default:
            printf("unsupported pixel depth: %i\n", type);
            *out = (GLvoid *)data;
            return format;
            break;
    }
#undef convert
    return newFormat;
}

void glTexImage2D(GLenum target, GLint level, GLint internalFormat,
                  GLsizei width, GLsizei height, GLint border,
                  GLenum format, GLenum type, const GLvoid *data) {

    // TODO: need to take GLenum type into account
    GLvoid *pixels;
    switch (format) {
        case GL_ALPHA:
        case GL_RGB:
        case GL_RGBA:
        case GL_LUMINANCE:
        case GL_LUMINANCE_ALPHA:
            pixels = (GLvoid *)data;
            break;
        default:
            format = convertPixels(data, &pixels, width, height, format, type);
            break;
    }

    LOAD_REAL(void, glTexImage2D, GLenum, GLint, GLint,
              GLsizei, GLsizei, GLint,
              GLenum, GLenum, const GLvoid*);
    real_glTexImage2D(target, level, format, width, height, border,
                      format, type, pixels);

    if (pixels != data) {
        free((GLvoid *)pixels);
    }
}
