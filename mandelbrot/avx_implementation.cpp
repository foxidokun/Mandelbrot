#include <assert.h>
#include <immintrin.h>
#include <stdint.h>

#include "config.h"
#include "avx_implementation.h"
#include <stdio.h>

const __m256 OFFSETS = _mm256_set_ps( 7*PIXEL_WIDTH, 6*PIXEL_WIDTH, 5*PIXEL_WIDTH, 4*PIXEL_WIDTH, 3*PIXEL_WIDTH,
                                                        2*PIXEL_WIDTH, PIXEL_WIDTH, 0.0);

void avx::calc(unsigned int width, unsigned int height, uint32_t *counters) {
    assert ((uint64_t) counters % 32 == 0 && "Wrong alignment");

    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; x += 8) {
            __m256 x_0 = _mm256_set1_ps((float) x * PIXEL_WIDTH + offset_x);
            x_0 = _mm256_add_ps(x_0, OFFSETS);

            __m256 y_0 = _mm256_set1_ps((float) y * PIXEL_WIDTH + offset_y);
            __m256 MAX_R2_vec = _mm256_set1_ps(MAX_R2);

            __m256 x_cur = x_0;
            __m256 y_cur = y_0;

            __m256i counters_vec = _mm256_setzero_si256();

            int i = 0;
            for (i = 0; i < MAX_STEPS; ++i) {
                __m256 x2 = _mm256_mul_ps(x_cur, x_cur);
                __m256 y2 = _mm256_mul_ps(y_cur, y_cur);
                __m256 two_xy = _mm256_mul_ps(x_cur, y_cur);
                two_xy = _mm256_add_ps(two_xy, two_xy);

                __m256 radius = _mm256_add_ps(x2, y2);

                __m256i compare_res = (__m256i) _mm256_cmp_ps(radius, MAX_R2_vec, _CMP_LT_OS);
                counters_vec = _mm256_sub_epi32(counters_vec, compare_res);

                if (_mm256_testz_si256(compare_res, compare_res) == 1) {
                    break;
                }

                x_cur = _mm256_add_ps(_mm256_sub_ps(x2, y2), x_0);
                y_cur = _mm256_add_ps(two_xy, y_0);
            }

            _mm256_store_si256((__m256i *) (counters + y*width + x), counters_vec);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void avx::overloaded_operators_calc(unsigned int width, unsigned int height, uint32_t *counters) {
    assert ((uint64_t) counters % 32 == 0 && "Wring alignment");

    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; x += 8) {
            __m256 x_0 = _mm256_set1_ps((float) x * PIXEL_WIDTH + offset_x);
            x_0 += OFFSETS;

            __m256 y_0 = _mm256_set1_ps((float) y * PIXEL_WIDTH + offset_y);
            __m256 MAX_R2_vec = _mm256_set1_ps(MAX_R2);

            __m256 x_cur = x_0;
            __m256 y_cur = y_0;

            __m256i counters_vec = {};

            int i = 0;
            for (i = 0; i < MAX_STEPS; ++i) {
                __m256 x2 = x_cur * x_cur;
                __m256 y2 = y_cur * y_cur;
                __m256 two_xy = x_cur * y_cur;
                two_xy = two_xy + two_xy;

                __m256 radius = x2 + y2;

                __m256i compare_res = (__m256i) _mm256_cmp_ps(radius, MAX_R2_vec, _CMP_LT_OS);
                counters_vec -= compare_res;

                if (_mm256_testz_si256(compare_res, compare_res) == 1) {
                    break;
                }

                x_cur = x2 - y2 + x_0;
                y_cur = two_xy + y_0;
            }

            _mm256_store_si256((__m256i *) (counters + y*width + x), counters_vec);
        }
    }
}
