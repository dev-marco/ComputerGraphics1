#include <string>
#include <cstdio>
#include <GL/glew.h>
#include <png.h>

#define ALIGN(i, size) (((size) - 1) - ((i) - 1) % (size))

GLuint loadPNG (const std::string &filename, png_uint_32 &width, png_uint_32 &height, int &bit_depth, int &color_type) {

    FILE *png_file = fopen(filename.c_str(), "rb");
    int row_bytes;
    png_byte *gl_image, **png_image;
    GLuint texture;
    GLint format;

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    png_infop info = png_create_info_struct(png);
    png_infop _tmp = png_create_info_struct(png);
    png_init_io(png, png_file);

    png_read_info(png, info);

    png_get_IHDR(png, info, &width, &height, &bit_depth, &color_type, nullptr, nullptr, nullptr);

    if (bit_depth != 8) {
        throw std::string("Bit depth not supported.");
    }

    png_read_update_info(png, info);

    row_bytes = png_get_rowbytes(png, info);

    row_bytes = ALIGN(row_bytes, 4);

    gl_image = new png_byte[row_bytes * height];
    png_image = new png_byte*[height];

    for (unsigned i = 0; i < height; i++) {
        png_image[height - i - 1] = gl_image + (i * row_bytes);
    }

    png_read_image(png, png_image);

    fclose(png_file);

    delete[] png_image;
    png_destroy_read_struct(&png, &info, &_tmp);

    format = color_type == PNG_COLOR_TYPE_RGB_ALPHA ? GL_RGBA : GL_RGB;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, gl_image);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    delete[] gl_image;

    return texture;
}
