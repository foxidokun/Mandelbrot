#include <assert.h>
#include <string.h>
#include <immintrin.h>

#include "avx_implementation.h"

// ---------------------------------------------------------------------------------------------------------------------

typedef unsigned int uint;

// ---------------------------------------------------------------------------------------------------------------------

// _mm256_packs_epi16

const __m256i ZERO_VECTOR = _mm256_setzero_si256();

const __m256i SHUFFLE_LOWERCASE_ARRAYS_VECTOR = _mm256_set_epi8(
        0x80,  23,  0x80, 22,
        0x80,  21,  0x80, 20,
        0x80,  19,  0x80, 18,
        0x80,  17,  0x80, 16,
        0x80,   7,  0x80,  6,
        0x80,   5,  0x80,  4,
        0x80,   3,  0x80,  2,
        0x80,   1,  0x80,  0
        );

const __m256i SHUFFLE_EXTRACT_ALPHA_VECTOR = _mm256_set_epi8(
        0x80, 30, 0x80, 30, 0x80, 30, 0x80, 30,
        0x80, 22, 0x80, 22, 0x80, 22, 0x80, 22,
        0x80, 14, 0x80, 14, 0x80, 14, 0x80, 14,
        0x80,  6, 0x80,  6, 0x80,  6, 0x80,  6
        );

const __m256i SHUFFLE_LOWERCASE_BACK_TO_PACKED_STATE = _mm256_set_epi8(0x80, 0x80, 0x80, 0x80,
                                                                       0x80, 0x80, 0x80, 0x80,
                                                                       31, 29, 27, 25,
                                                                       23, 21, 19, 17,
                                                                       0x80, 0x80, 0x80, 0x80,
                                                                       0x80, 0x80, 0x80, 0x80,
                                                                       15, 13, 11, 9,
                                                                       7, 5, 3, 1);

const __m256i SHUFFLE_UPPERCASE_BACK_TO_PACKED_STATE = _mm256_set_epi8(31, 29, 27, 25,
                                                                       23, 21, 19, 17,
                                                                       0x80, 0x80, 0x80, 0x80,
                                                                       0x80, 0x80, 0x80, 0x80,
                                                                       15, 13, 11, 9,
                                                                       7, 5, 3, 1,
                                                                       0x80, 0x80, 0x80, 0x80,
                                                                       0x80, 0x80, 0x80, 0x80
                                                                       );
void vector::mix(const image_t *background, const image_t *foreground, image_t *out_image) {
#ifdef SAME_SIZE_ONLY
    assert (background->width == foreground->width && foreground->width == out_image->width && "Built with SAME_SIZE_ONLY => expected same size images");
    assert (background->height == foreground->height && foreground->height == out_image->height && "Built with SAME_SIZE_ONLY => expected same size images");
#endif

    pixel_t *b_pixels = background->pixels;
    pixel_t *f_pixels = foreground->pixels;
    pixel_t *o_pixels = out_image->pixels;

    uint f_width  = foreground->width;
    uint f_height = foreground->height;
    uint b_width  = background->width;

    for (uint y = 0; y < f_height; ++y) {
        for (uint x = 0; x < f_width; x += 8) {
            // fr/bg array:
            // [  0  1  2  3  4  5  6  7 ... | 16 17 18 19 20 21 22 23 ... ]
            // [ r1 g1 b1 a1 r2 g2 b2 a2 ... | r5 g5 b5 a5 r6 g6 b6 a6 ... ]

            __m256i fr = _mm256_loadu_si256((__m256i *) (f_pixels + y*f_width + x));
            __m256i bg = _mm256_loadu_si256((__m256i *) (b_pixels + y*b_width + x));

            // FR/BG array:
            // [  0  1  2  3  4  5  6  7 ... | 16 17 18 19 20 21 22 23 ... ]
            // [ r3  0 g3  0 b3  0 a3  0 ... | r7  0 g7  0 b7  0 a7  0 ... ]
            __m256i FR = _mm256_unpackhi_epi8( fr, ZERO_VECTOR);
            __m256i BG = _mm256_unpackhi_epi8( bg, ZERO_VECTOR);

            // fr/bg array:
            // [  0  1  2  3  4  5  6  7 ... | 16 17 18 19 20 21 22 23 ... ]
            // [ r1  0 g1  0 b1  0 a1  0 ... | r5  0 g5  0 b5  0 a5  0 ... ] and
            // [ r3  0 g3  0 b3  0 a3  0 ... | r7  0 g7  0 b7  0 a7  0 ... ]
            fr = _mm256_shuffle_epi8(fr, SHUFFLE_LOWERCASE_ARRAYS_VECTOR);
            bg = _mm256_shuffle_epi8(bg, SHUFFLE_LOWERCASE_ARRAYS_VECTOR);

            // alpha/ALPHA array:
            // [  0  1  2  3  4  5  6  7 ... | 16 17 18 19 20 21 22 23 ... ]
            // [ a1  0 a1  0 a1  0 a1  0 ... | a5  0 a5  0 a5  0 a5  0 ... ]
            __m256i alpha = fr;
            __m256i ALPHA = FR;
            alpha = _mm256_shuffle_epi8(alpha, SHUFFLE_EXTRACT_ALPHA_VECTOR);
            ALPHA = _mm256_shuffle_epi8(ALPHA, SHUFFLE_EXTRACT_ALPHA_VECTOR);

            fr = _mm256_mullo_epi16 (fr, alpha);
            FR = _mm256_mullo_epi16 (FR, ALPHA);

            // Multiply background to 255-alpha
            __m256i max_alpha = _mm256_set1_epi16(255);
            alpha = _mm256_sub_epi16(max_alpha, alpha);
            ALPHA = _mm256_sub_epi16(max_alpha, ALPHA);

            bg = _mm256_mullo_epi16 (bg, alpha);
            BG = _mm256_mullo_epi16 (BG, ALPHA);

            // result/RESULT array:
            // [  0  1  2  3  4  5  6  7 ... | 16 17 18 19 20 21 22 23 ... ]
            // [ a1  0 r1  0 g1  0 b1  0 ... | a5  0 r5  0 g5  0 b5  0 ... ] and
            // [ a3  0 r3  0 g3  0 b3  0 ... | a7  0 r7  0 g7  0 b7  0 ... ]
            __m256i result = _mm256_add_epi16(fr, bg);
            __m256i RESULT = _mm256_add_epi16(FR, BG);

            // [  0  1  2  3  4  5  6  7 ... | 16 17 18 19 20 21 22 23  ... ]
            // [ a1 r1 g1 b1  0  0  0  0 ... | a5 r5 g5 b5  0  0  0  0  ... ] and
            // [  0  0  0  0 a3 r3 g3 b3 ... |  0  0  0  0 a7 r7 g7 b7  ... ]
            result = _mm256_shuffle_epi8(result, SHUFFLE_LOWERCASE_BACK_TO_PACKED_STATE);
            RESULT = _mm256_shuffle_epi8(RESULT, SHUFFLE_UPPERCASE_BACK_TO_PACKED_STATE);

            // Combine
            result = _mm256_or_si256(result, RESULT);

            _mm256_storeu_si256((__m256i *) (o_pixels + y*b_width + x), result);
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
