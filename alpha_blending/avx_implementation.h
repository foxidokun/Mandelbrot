#ifndef MANDELBROT_AVX_IMPLEMENTATION_H
#define MANDELBROT_AVX_IMPLEMENTATION_H

#include "image.h"

namespace vector {
    void mix(const image_t *background, const image_t *foreground, image_t *out_image);
}


#endif //MANDELBROT_AVX_IMPLEMENTATION_H
