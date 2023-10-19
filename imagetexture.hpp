#pragma once

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GLFW/glfw3.h>

#include <VmbC/VmbC.h>

#include <mutex>

// #define eprintf(fmt, ...)                                                                 \
//     {                                                                                     \
//         fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
//         fflush(stderr);                                                                   \
//     }

#define eprintf(fmt, ...)

class Image
{
private:
    uint32_t width;
    uint32_t height;
    uint8_t *data;
    VmbPixelFormat_t pixelFormat;
    GLuint texture = 0;
    GLenum fmt;
    GLenum type;
    bool reset = false;
    bool newdata = false;
    std::mutex mtx;

public:
    Image()
    {
        width = 0;
        height = 0;
        texture = 0;
        fmt = GL_LUMINANCE;
        type = GL_UNSIGNED_BYTE;
        pixelFormat = VmbPixelFormatMono8;
    }

    ~Image()
    {
        if (texture)
            glDeleteTextures(1, &texture);
    }

    void get_texture(GLuint &tex, uint32_t &w, uint32_t &h) // draw in the ImGui thread
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (!newdata)
        {
            tex = texture;
            w = width;
            h = height;
            return;
        }
        newdata = false;
        if (reset)
        {
            pixfmt_to_glfmt(pixelFormat, fmt, type);
            eprintf("Image: %u x %u | %u | %u | %u\n", width, height, pixelFormat, fmt, type);
            if (texture)
            {
                glDeleteTextures(1, &texture);
                eprintf("Deleted texture: %d\n", texture);
            }
            GLuint itexture;
            glGenTextures(1, &itexture);
            eprintf("Generated texture: %d\n", itexture);
            glBindTexture(GL_TEXTURE_2D, itexture);
            eprintf("Bound texture: %d\n", itexture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, fmt, width, height, 0, fmt, type, data);
            eprintf("Set texture: %d | %u x %u\n", itexture, width, height);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            eprintf("Created texture: %d | %u x %u\n", itexture, width, height);
            texture = itexture;
            reset = false;
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, texture);
            eprintf("Rebound texture: %d\n", texture);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, fmt, type, data);
            eprintf("Updated texture: %d | %u x %u\n", texture, width, height);
        }
        tex = texture;
        w = width;
        h = height;
    }

    void update(VmbFrame_t *frame)
    {
        if (data == frame->buffer)
        {
            // reading from this frame, lock!
            std::lock_guard<std::mutex> lock(mtx);
            // drop the lock after this
        }
        // now try to lock again and do the update
        if (!mtx.try_lock())
        {
            return;
        }
        eprintf("Updating image\n");
        if (width != frame->width || height != frame->height || pixelFormat != frame->pixelFormat)
        {
            reset = true;
        }
        width = frame->width;
        height = frame->height;
        pixelFormat = frame->pixelFormat;
        data = (uint8_t *)frame->buffer;
        newdata = true;
        mtx.unlock();
    }

    static void update(Image *self, VmbFrame_t *frame)
    {
        self->update(frame);
    }

    static void pixfmt_to_glfmt(VmbPixelFormat_t pfmt, GLenum &fmt, GLenum &type)
    {
        switch (pfmt)
        {
        case VmbPixelFormatMono8:
            fmt = GL_LUMINANCE;
            type = GL_UNSIGNED_BYTE;
            break;
        case VmbPixelFormatMono10:
            fmt = GL_LUMINANCE;
            type = GL_UNSIGNED_SHORT;
            break;
        case VmbPixelFormatMono10p:
            fmt = GL_LUMINANCE;
            type = GL_UNSIGNED_SHORT;
            break;
        case VmbPixelFormatMono12:
            fmt = GL_LUMINANCE;
            type = GL_UNSIGNED_SHORT;
            break;
        case VmbPixelFormatMono14:
            fmt = GL_LUMINANCE;
            type = GL_UNSIGNED_SHORT;
            break;
        case VmbPixelFormatMono16:
            fmt = GL_LUMINANCE;
            type = GL_UNSIGNED_SHORT;
            break;
        case VmbPixelFormatBgr8:
            fmt = GL_BGR;
            type = GL_UNSIGNED_BYTE;
            break;
        case VmbPixelFormatBgra8:
            fmt = GL_BGRA;
            type = GL_UNSIGNED_BYTE;
            break;
        case VmbPixelFormatRgb8:
            fmt = GL_RGB;
            type = GL_UNSIGNED_BYTE;
            break;
        case VmbPixelFormatRgba8:
            fmt = GL_RGBA;
            type = GL_UNSIGNED_BYTE;
            break;
        case VmbPixelFormatRgb16:
            fmt = GL_RGB;
            type = GL_UNSIGNED_SHORT;
            break;
        case VmbPixelFormatBgr16:
            fmt = GL_BGR;
            type = GL_UNSIGNED_SHORT;
            break;
        case VmbPixelFormatRgba16:
            fmt = GL_RGBA;
            type = GL_UNSIGNED_SHORT;
            break;
        case VmbPixelFormatBgra16:
            fmt = GL_BGRA;
            type = GL_UNSIGNED_SHORT;
            break;
        default:
            fmt = GL_LUMINANCE;
            type = GL_UNSIGNED_BYTE;
            break;
        }
    }
};
