#include "config.h"
#include "graphics.h"

// ---------------------------------------------------------------------------------------------------------------------

const int RED_MULTIPLY   = 15;
const int GREEN_MULTIPLY = 40;
const int BLUE_MULTIPLY  = 30;

// ---------------------------------------------------------------------------------------------------------------------

static void convert_to_colors (gui_t *self, const uint32_t *counters);

// ---------------------------------------------------------------------------------------------------------------------

void gui::render(gui_t *self, uint32_t *counters) {
    convert_to_colors (self, counters);

    self->window.clear();
    self->pixel_texture.update((uint8_t *) self->buf);

    self->window.draw(self->pixel_sprite);
    self->window.display();

    sf::Event event;
    while (self->window.isOpen()){
	    while (self->window.pollEvent(event))
	    {
	        if (event.type == sf::Event::Closed) {
	            self->window.close();
	            return;
	        }
	    }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void gui::ctor(struct gui_t *self, unsigned int width, unsigned int height, const char *title) {
    self->window.create(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), title);
    self->pixel_texture.create(width, height);
    self->pixel_sprite.setTexture (self->pixel_texture);

    self->height = height;
    self->width = width;
    self->buf = (pixel_t *) calloc(width * height, sizeof(pixel_t));
}

// ---------------------------------------------------------------------------------------------------------------------

static void convert_to_colors (gui_t *self, const uint32_t *counters) {
    uint height = self->height;
    uint width = self->width;
    pixel_t *buf = self->buf;

    for (uint y = 0; y < height; ++y) {
        for (uint x = 0; x < width; ++x) {
            uint index = y * width + x;
            buf[index] = {	.red  = (uint8_t) ((counters[index] * 15) % 256),
            				.green= (uint8_t) ((counters[index] * 40) % 256),
            				.blue = (uint8_t) ((counters[index] * 30) % 256),
            				.alpha=255};
        }
    }
}
