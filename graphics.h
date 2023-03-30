#ifndef MANDELBROT_GRAPHICS_H
#define MANDELBROT_GRAPHICS_H

#include <SFML/Graphics.hpp>

struct pixel_t {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
};

struct gui_t {
    sf::RenderWindow window; // ?
    sf::Texture pixel_texture;
    sf::Sprite pixel_sprite;

    pixel_t *buf; // 8


    unsigned int height; // 4
    unsigned int width; // 4
};

namespace gui {
    void ctor(struct gui_t *self, unsigned int width, unsigned int height, const char *title);
    static void dtor(gui_t *self) {} // Do nothing right now

    void render(gui_t *self, uint32_t *counters);
};

#endif //MANDELBROT_GRAPHICS_H
