#include <assert.h>

#include "scalar_implementation.h"

// ---------------------------------------------------------------------------------------------------------------------

typedef unsigned int uint;

// ---------------------------------------------------------------------------------------------------------------------

void scalar::mix(image_t *background, const image_t *foreground) {
    assert(background->height >= foreground->height && "Foreground can't be smaller");
    assert(background->width >= foreground->width && "Foreground can't be smaller");

    uint f_width = foreground->width;
    uint b_width = background->width;

    pixel_t *f_pixels = foreground->pixels;
    pixel_t *b_pixels = background->pixels;

    for (uint y = 0; y < foreground->height; ++y) {
        for (uint x = 0; x < foreground->width; ++x) {
            pixel_t back_pixel = b_pixels[y * b_width + x];
            pixel_t front_pixel = f_pixels[y * f_width + x];
            int remaining_alpha = 255 - front_pixel.a;

            b_pixels[y * b_width + x].a = 255;
            b_pixels[y * b_width + x].r = (front_pixel.r * front_pixel.a + back_pixel.r * remaining_alpha) >> 8;
            b_pixels[y * b_width + x].g = (front_pixel.g * front_pixel.a + back_pixel.g * remaining_alpha) >> 8;
            b_pixels[y * b_width + x].b = (front_pixel.b * front_pixel.a + back_pixel.b * remaining_alpha) >> 8;
        }
    }
}