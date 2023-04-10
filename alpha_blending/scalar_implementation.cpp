#include <assert.h>
#include <string.h>

#include "scalar_implementation.h"

// ---------------------------------------------------------------------------------------------------------------------

typedef unsigned int uint;

// ---------------------------------------------------------------------------------------------------------------------

void scalar::mix(const image_t *background, const image_t *foreground, image_t *out_image) {
    assert(background->height >= foreground->height && "Foreground can't be smaller");
    assert(background->width >= foreground->width && "Foreground can't be smaller");

    uint f_width = foreground->width;
    uint f_height = foreground->height;
    uint b_width = background->width;

    pixel_t *f_pixels = foreground->pixels;
    pixel_t *b_pixels = background->pixels;
    pixel_t *o_pixels = out_image->pixels;

    for (uint y = 0; y < f_height; ++y) {
        for (uint x = 0; x < f_width; ++x) {
            pixel_t back_pixel = b_pixels[y * b_width + x];
            pixel_t front_pixel = f_pixels[y * f_width + x];
            int remaining_alpha = 255 - front_pixel.a;

            o_pixels[y * b_width + x].a = 255;
            o_pixels[y * b_width + x].r = (front_pixel.r * front_pixel.a + back_pixel.r * remaining_alpha) >> 8;
            o_pixels[y * b_width + x].g = (front_pixel.g * front_pixel.a + back_pixel.g * remaining_alpha) >> 8;
            o_pixels[y * b_width + x].b = (front_pixel.b * front_pixel.a + back_pixel.b * remaining_alpha) >> 8;
        }

#ifndef SAME_SIZE_ONLY
        int line_end = y*b_width + f_width;
        memcpy (o_pixels + line_end, b_pixels + line_end, (b_width - f_width) * BYTES_PER_PIXEL);
#endif
    }

#ifndef SAME_SIZE_ONLY
    int file_end = f_height*b_width;
    memcpy (o_pixels + file_end, b_pixels + file_end, b_width * (background->height - f_height) * BYTES_PER_PIXEL);
#endif
}