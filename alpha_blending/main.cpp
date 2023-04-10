#include "image.h"
#include "scalar_implementation.h"
#include "avx_implementation.h"
#include "stdio.h"

#include <sys/time.h>
#include <math.h>

#include "config.h"
#include "scalar_implementation.h"
#include "avx_implementation.h"

typedef void (*mix_func_t)(const image_t *background, const image_t *foreground, image_t *out_image);

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

template<mix_func_t mix_func>
void bench_mix(const char *func_name) {
#ifndef SAME_SIZE_ONLY
    fprintf (stderr, "Warning: bencmarking without SAME_SIZE_ONLY option: memcpys will affect the result");
#endif

    unsigned long iteration_us[ITERATION_NUM] = {};
    struct timeval start, stop;

    image_t* front = image::open(FOREGROUND_FILE, file_open_mode::READ_ONLY);
    image_t* back  = image::open(BACKGROUND_FILE, file_open_mode::READ_ONLY);

    image_t *out_image = image::create(back->width, back->height);

    for (int i = 0; i < ITERATION_NUM; ++i) {

        gettimeofday(&start, NULL);
        mix_func(back, front, out_image);
        gettimeofday(&stop, NULL);

        image::save(out_image, RESULT_FILE);

        iteration_us[i] = (unsigned long) ((stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec));
    }

    image::close(out_image);
    image::close(back);
    image::close(front);

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
#ifdef AVX_VERSION
    bench_mix<vector::mix>("vector::mix");
#elif defined(SCALAR_VERSION)
    bench_mix<scalar::mix>("scalar::mix");
#else
    #error "No version"
#endif
}
