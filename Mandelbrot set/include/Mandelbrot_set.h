#ifndef MANDELBROT_SET_H
    #define MANDELBROT_SET_H

    #include <SFML/Graphics.hpp>
    #include <x86intrin.h>

    struct PixelCs {
        float cs_x;
        float cs_y;
    };

    struct PixelCsVect {
        __m128 cs_x_vect;
        __m128 cs_y_vect;
    };

    void framerate_text_ctor(sf::Text * framerate, sf::Font * font, const char * font_file);
    void simd_mandelbrot_draw(sf::VertexArray * set, const PixelCs cs_centre,
                              const float scale);
    void default_mandelbrot_draw(sf::VertexArray * set, const PixelCs cs_centre,
                                 const float scale);
    int default_calculate_pixel_color(float x, float y, const PixelCs cs_centre,
                                      const float scale);
    __m128i simd_calculate_pixel_color(float x, float y, const PixelCs cs_centre,
                                      const float scale);

    const size_t TEXT_SIZE_COEFFICIENT = 19200;             // Ratio of screen area to text size.
    const size_t SCREEN_WIDTH = 4 * 400;                    
    const size_t SCREEN_HIGHT = 1200;
    const size_t R_MAX        = 500;                        // Mandelbrot set border.
    const size_t FRAMERATE_TEXT_SIZE = SCREEN_WIDTH * SCREEN_HIGHT / TEXT_SIZE_COEFFICIENT;
    const float  ELEM_SCREEN_OFFSET = 90;                   // One-touch screen shift.
    const float  SCALE_COEF = 1.1f;                         // One-touch screen scaling.

#endif // MANDELBROT_SET_H