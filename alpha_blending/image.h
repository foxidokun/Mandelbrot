#ifndef MANDELBROT_IMAGE_H
#define MANDELBROT_IMAGE_H

#include <stdint.h>

typedef unsigned int uint;

struct __attribute__((__packed__)) bmp_header_t {
    unsigned char signature_marker_1;
    unsigned char signature_marker_2;
    uint32_t file_size;
    uint32_t reserved;
    uint32_t pixel_data_offset;

    // Image Header
    uint32_t image_header_size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bits_per_pixel;

    // Not needed data
    uint32_t compression_level;
    uint32_t image_size;
    uint32_t XPelsPerMeter;
    uint32_t YPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};

typedef struct __attribute__((__packed__)) pixel_t {
    unsigned char  r;
    unsigned char  g;
    unsigned char  b;
    unsigned char  a;
};

enum class file_open_mode {
    READ_ONLY = 1,
    READ_WRITE = 2
};

struct image_t {
    unsigned int width;
    unsigned int height;
    unsigned int byte_size;
    pixel_t *pixels;
    void *mmap_addr;
};

namespace image {
    image_t *open(const char *filename, file_open_mode mode);
    image_t *create(uint width, uint height);
    void free(image_t *self);
    void close(image_t *self);

    int save(image_t *self, const char *filename);
}

#endif //MANDELBROT_IMAGE_H
