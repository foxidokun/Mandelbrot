#ifndef MANDELBROT_AVX_IMPLEMENTATION_H
#define MANDELBROT_AVX_IMPLEMENTATION_H

namespace avx {
    void calc (unsigned int width, unsigned int height, uint32_t *counters);
    void overloaded_operators_calc(unsigned int width, unsigned int height, uint32_t *counters);
}

#endif //MANDELBROT_AVX_IMPLEMENTATION_H
