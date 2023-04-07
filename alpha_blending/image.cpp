#include <assert.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "image.h"

// ---------------------------------------------------------------------------------------------------------------------

const int BYTES_PER_PIXEL = 4;

const int IMAGE_HEADER_SIZE = 40;
const int PIXEL_DATA_OFFSET = sizeof(bmp_header_t);

// ---------------------------------------------------------------------------------------------------------------------

#define check_format(cond) {    \
    if (!(cond)) {              \
        fprintf(stderr, "Invalid file format: unexpected " #cond " in %s\n", filename); \
        return nullptr;         \
    }                           \
}

image_t *image::open(const char *filename, file_open_mode mode) {
    struct bmp_header_t header = {};

    int fd = ::open(filename, O_RDONLY);

    if (fd == -1) {
        fprintf(stderr, "failed to open file %s with error %s\n", filename, strerror(errno));
        return nullptr;
    }

    read(fd, &header, sizeof(header));

    check_format(header.compression_level == 0);
    check_format(header.bits_per_pixel == 32);

    image_t *self = (image_t *) malloc (sizeof(image_t));
    self->height    = header.height;
    self->width     = header.width;
    self->byte_size = header.width * header.height * BYTES_PER_PIXEL;

    int mmap_mode = -1;
    switch (mode) {
        case file_open_mode::READ_ONLY:
            mmap_mode = PROT_READ;
            break;
        case file_open_mode::READ_WRITE:
            mmap_mode = PROT_READ | PROT_WRITE;
            break;
        default:
            assert (0 && "Unexpected enum member");
    }

    self->mmap_addr = mmap(nullptr, self->byte_size, mmap_mode, MAP_PRIVATE, fd, 0);
    self->pixels = (pixel_t *) ((char *)self->mmap_addr + header.pixel_data_offset);

    if (self->mmap_addr == MAP_FAILED) {
        fprintf(stderr, "failed to mmap file %s with error: %s\n", filename, strerror(errno));
        free(self);
        return nullptr;
    }

    return self;
}

#undef check_format

// ---------------------------------------------------------------------------------------------------------------------

image_t *image::create(uint width, uint height) {
    image_t *self = (image_t *) calloc(1, sizeof (image_t));
    self->height = height;
    self->width  = width;
    self->byte_size = width * height * BYTES_PER_PIXEL;

    self->pixels = (pixel_t *) calloc(width * height, sizeof(pixel_t));
    self->mmap_addr = nullptr;

    return self;
}

// ---------------------------------------------------------------------------------------------------------------------

void image::close(image_t *self) {
    munmap(self->mmap_addr, self->byte_size);
    ::free(self);
}

void image::free(image_t *self) {
    ::free(self->pixels);
    ::free(self);
}

// ---------------------------------------------------------------------------------------------------------------------

int image::save(image_t *self, const char *filename) {
    int fd = creat (filename, O_RDWR);

    if (fd == -1) {
        fprintf(stderr, "failed to save file %s with error %s\n", filename, strerror(errno));
        return ERROR;
    }

    bmp_header_t header = {
            .signature_marker_1 = 'B',
            .signature_marker_2 = 'M',
            .file_size = 0, // Cuz why not
            .pixel_data_offset = PIXEL_DATA_OFFSET,
            .image_header_size = IMAGE_HEADER_SIZE,
            .width  = self->width,
            .height = self->height,
            .planes = 1,               // Specified by standart
            .bits_per_pixel    = BYTES_PER_PIXEL * 8,   // 4 bytes of RGBA
            .compression_level = 0,
            .image_size = self->byte_size
    };

    write(fd, &header, sizeof(header));
    write(fd, self->pixels, self->byte_size);

    return 0;
}