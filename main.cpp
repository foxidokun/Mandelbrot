#include <sys/time.h>
#include <math.h>

#include "config.h"
#include "graphics.h"
#include "dumb_version.h"
#include "avx_realisation.h"

typedef void (*calc_func)(unsigned int width, unsigned int height, uint32_t *counters);

template<calc_func mandelbrot_function>
void bench_calc(uint width, uint height, uint32_t *counters, const char *func_name) {
    unsigned long iteration_us[ITERATION_NUM] = {};
    struct timeval start, stop;

    for (int i = 0; i < ITERATION_NUM; ++i) {
        gettimeofday(&start, NULL);
        mandelbrot_function(width, height, counters);
        gettimeofday(&stop, NULL);

        iteration_us[i] = (unsigned long) ((stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec));
    }

    long long unsigned mean = 0;
    for (int i = 0; i < ITERATION_NUM; ++i) {
        mean += (unsigned long) iteration_us[i];
    }
    mean /= ITERATION_NUM;

    double sigma = 0;
    for (int i = 0; i < ITERATION_NUM; ++i) {
        sigma += (double) ((iteration_us[i] - mean) * (iteration_us[i] - mean));
    }
    sigma /= ITERATION_NUM;
    sigma = sqrt(sigma);

    printf ("Benchmark results (%s): %llu ± %lg ms (%d iterations)\n",
            func_name, mean / 1000, sigma / 1000, ITERATION_NUM
    );
}

int main() {
    uint32_t *counters = (uint32_t *) aligned_alloc(32, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));

    bench_calc<avx::calc>(SCREEN_WIDTH, SCREEN_HEIGHT, counters, "avx::calc");
// bench_calc<dumb::calc>(SCREEN_WIDTH, SCREEN_HEIGHT, counters, "dumb::calc");

    gui_t window;
    gui::ctor(&window, SCREEN_WIDTH, SCREEN_HEIGHT, "Mandelbrot");
    gui::render(&window, counters);

    gui::dtor(&window);
}
