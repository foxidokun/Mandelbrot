#include "image.h"
#include "scalar_implementation.h"
#include "avx_implementation.h"

const char BACKGROUND_FILE[] = "background.bmp";
const char FOREGROUND_FILE[] = "foreground.bmp";

int main() {
    image_t* back   = image::open(BACKGROUND_FILE, file_open_mode::READ_WRITE);
    image_t* front  = image::open(FOREGROUND_FILE, file_open_mode::READ_ONLY);

    vector::mix (back, front);

    image::save(back, "result.bmp");
    image::close(back);
    image::close(front);
}