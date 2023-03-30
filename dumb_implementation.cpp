#include "config.h"

#include "dumb_implementation.h"

// ---------------------------------------------------------------------------------------------------------------------

void dumb::calc (unsigned int width, unsigned int height, uint32_t *counters) {
    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; ++x) {
            float x_0 = (float) x * PIXEL_WIDTH + offset_x;
            float y_0 = (float) y * PIXEL_WIDTH + offset_y;

            float x_cur = x_0;
            float y_cur = y_0;

            int i = 0;
            for (i = 0; i < MAX_STEPS; ++i) {
                float x2     = x_cur * x_cur;
                float y2     = y_cur * y_cur;
                float two_xy = 2 * x_cur * y_cur;

                if (x2 + y2 > MAX_R2) {
                    break;
                }

                x_cur = x2 - y2 + x_0;
                y_cur = two_xy + y_0;
            }

            counters[y*width + x] = i;
        }
    }
}