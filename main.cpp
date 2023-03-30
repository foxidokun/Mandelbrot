#include <sys/time.h>
#include <math.h>

#include "config.h"
#include "graphics.h"
#include "dumb_implementation.h"
#include "avx_implementation.h"

typedef void (*calc_func)(unsigned int width, unsigned int height, uint32_t *counters);

#ifdef __clang__
	const char *COMPILER_NAME = "Clang";
    const int MAJOR_VERSION = __clang_major__;
    const int MINOR_VERSION = __clang_minor__;
    const int PATCH_VERSION = __clang_patchlevel__;
#elif __GNUC__
    const char COMPILER_NAME[] = "GCC";
    const int MAJOR_VERSION = __GNUC__;
    const int MINOR_VERSION = __GNUC_MINOR__;
    const int PATCH_VERSION = __GNUC_PATCHLEVEL__;
#else
    const char *COMPILER_NAME = "Unknown";
    const int MAJOR_VERSION = 0;
    const int MINOR_VERSION = 0;
    const int PATCH_VERSION = 0;
#endif

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

    printf ("Benchmark results (%s): %llu ± %lg ms (%d iterations, %s v%d.%d.%d)\n",
            func_name, mean / 1000, sigma / 1000, ITERATION_NUM, COMPILER_NAME, MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION
    );
}	

int main() {
    uint32_t *counters = (uint32_t *) aligned_alloc(32, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));

	#ifdef AVX_VERSION
    	bench_calc<avx::calc>(SCREEN_WIDTH, SCREEN_HEIGHT, counters, "avx::calc");
    #elif defined(DUMB_VERSION)
		bench_calc<dumb::calc>(SCREEN_WIDTH, SCREEN_HEIGHT, counters, "dumb::calc");
	#else
		#error You must choose one variant
	#endif

    gui_t window;
    gui::ctor(&window, SCREEN_WIDTH, SCREEN_HEIGHT, "Mandelbrot");
    gui::render(&window, counters);

    gui::dtor(&window);
}
