#include <math.h>

#include "Mandelbrot_set.h"
#include "my_assert.h"

const size_t VECTOR_DIM = 4;            // Vectors dimensionality.
const int MAX_CYCLE_COUNT = 255;
const float R_MAX2 = R_MAX * R_MAX;
const __m128 R_MAX2_VECT = _mm_set_ps1(R_MAX2);
const float EILER_NUMBER = 2.718f;
const float PI_NUMBER    = 3.142f;
const float GOLD_NUMBER  = 1.618f;

static inline sf::Color get_calculated_color(const int num_of_iterations);

int default_calculate_pixel_color(float x, float y, const PixelCs cs_centre,
                                  const float scale)
{
    MY_ASSERT(0 <= x && x < SCREEN_WIDTH);
    MY_ASSERT(0 <= y && y < SCREEN_HIGHT);

    const PixelCs pixel0 = {.cs_x =   (x - cs_centre.cs_x) * scale,
                            .cs_y = - (y - cs_centre.cs_y) * scale};
    PixelCs cur_pixel = pixel0;

    int i = 0;
    PixelCs cur_pixel2 = {.cs_x = cur_pixel.cs_x * cur_pixel.cs_x,
                          .cs_y = cur_pixel.cs_y * cur_pixel.cs_y};
    for (i = 0; i < MAX_CYCLE_COUNT; i++)
    {
        cur_pixel.cs_y = 2 * cur_pixel.cs_x * cur_pixel.cs_y + pixel0.cs_y; // y_{n+1} = 2 * y_n * x_n + y0
        cur_pixel.cs_x = cur_pixel2.cs_x - cur_pixel2.cs_y + pixel0.cs_x;   // x_{n+1} = x_n^2 - y_n^2 + x0

        cur_pixel2.cs_y = cur_pixel.cs_y * cur_pixel.cs_y;
        cur_pixel2.cs_x = cur_pixel.cs_x * cur_pixel.cs_x;

        if (cur_pixel2.cs_x + cur_pixel2.cs_y > R_MAX2)
            break;
    }
 
    return i;
}


__m128i simd_calculate_pixel_color(float x, float y, const PixelCs cs_centre,
                                   const float scale)
{
    MY_ASSERT(0 <= x && x <= SCREEN_WIDTH - 4);
    MY_ASSERT(0 <= y && y < SCREEN_HIGHT);

    PixelCsVect vector01 = {.cs_x_vect = _mm_set_ps1(  (x - cs_centre.cs_x) * scale),   // x_vect = || x - centr, x - centr, x - centr, x - centr ||
                            .cs_y_vect = _mm_set_ps1(- (y - cs_centre.cs_y) * scale)};  // y_vect = || y - centr, y - centr, y - centr, y - centr ||
    __m128 alignment = _mm_set_ps(3 * scale, 2 * scale, 1 * scale, 0 * scale);
    const PixelCsVect vector0 = {.cs_x_vect = _mm_add_ps(vector01.cs_x_vect, alignment),  // // x_vect = || x - centr + 3, x - centr + 2, x - centr + 1, x - centr ||
                                 .cs_y_vect = vector01.cs_y_vect};

    PixelCsVect cur_vector = vector0;
    
    PixelCsVect cur_vector2 = {_mm_mul_ps(cur_vector.cs_x_vect, cur_vector.cs_x_vect),
                               _mm_mul_ps(cur_vector.cs_y_vect, cur_vector.cs_y_vect)};
    __m128i i_vect = _mm_set1_epi32(0);
    __m128 cmp_res = _mm_set_ps1(0);
    __m128 imm_val = _mm_set_ps1(0);
    for (size_t i = 0; i < MAX_CYCLE_COUNT; i++)
    {
        imm_val = _mm_mul_ps(cur_vector.cs_x_vect, cur_vector.cs_y_vect);
        cur_vector.cs_y_vect = _mm_add_ps(_mm_add_ps(imm_val, imm_val), vector0.cs_y_vect);
        cur_vector.cs_x_vect = _mm_add_ps(_mm_sub_ps(cur_vector2.cs_x_vect, cur_vector2.cs_y_vect), vector0.cs_x_vect);

        cur_vector2.cs_x_vect = _mm_mul_ps(cur_vector.cs_x_vect, cur_vector.cs_x_vect);
        cur_vector2.cs_y_vect = _mm_mul_ps(cur_vector.cs_y_vect, cur_vector.cs_y_vect);

        imm_val = _mm_add_ps(cur_vector2.cs_x_vect, cur_vector2.cs_y_vect);
        cmp_res = _mm_cmple_ps(imm_val, R_MAX2_VECT);
        
        int mask = _mm_movemask_ps(cmp_res);
        if (!mask) break;                               // Break if all pixels out of bounds.

        i_vect = _mm_sub_epi32(i_vect, _mm_castps_si128(cmp_res));
    }

    return i_vect;
}


void default_mandelbrot_draw(sf::VertexArray * set, const PixelCs cs_centre,
                             const float scale)
{
    for (size_t i = 0; i < SCREEN_WIDTH; i++)
    {
        for (size_t j = 0; j < SCREEN_HIGHT; j++)
        {
            (*set)[(int)(SCREEN_WIDTH * j + i)].position = sf::Vector2f((float) i, (float) j);
            (*set)[(int)(SCREEN_WIDTH * j + i)].color = get_calculated_color(default_calculate_pixel_color((float) i, (float) j, cs_centre, scale));
        }
    }

    return;
}

void simd_mandelbrot_draw(sf::VertexArray * set, const PixelCs cs_centre,
                          const float scale)
{
    for (size_t i = 0; i < SCREEN_WIDTH; i += 4)
    {
        for (size_t j = 0; j < SCREEN_HIGHT; j++)
        {
            __m128i color_vector = simd_calculate_pixel_color((float) i, (float) j, cs_centre, scale);
            int * vector_ptr = (int *) &color_vector;
            int vals_arr[VECTOR_DIM] = {vector_ptr[0], vector_ptr[1],
                                        vector_ptr[2], vector_ptr[3]};

            for (size_t offset = 0; offset < 4; offset++)
            {
                (*set)[SCREEN_WIDTH * j + i + offset].position = sf::Vector2f((float) (i + offset), (float) (j));
                (*set)[SCREEN_WIDTH * j + i + offset].color = get_calculated_color(vals_arr[offset]);
            }
        }

    }

    return;
}


void framerate_text_ctor(sf::Text * framerate, sf::Font * font, const char * font_file)
{
    MY_ASSERT(framerate);

    font->loadFromFile(font_file);
    framerate->setFont(*font);
    framerate->setStyle(sf::Text::Style::Bold);
    framerate->setPosition(sf::Vector2f(SCREEN_WIDTH / 20, SCREEN_HIGHT / 25));
    framerate->setCharacterSize(FRAMERATE_TEXT_SIZE);
    framerate->setFillColor(sf::Color::Red);
}


static inline sf::Color get_calculated_color(const int num_of_iterations)
{
    return sf::Color((sf::Uint8) (EILER_NUMBER * (float) (num_of_iterations - 1) + (float) (num_of_iterations % 2) * 30),
                     (sf::Uint8) (GOLD_NUMBER  * (float) (num_of_iterations - 1)),
                     (sf::Uint8) (PI_NUMBER    * (float) (num_of_iterations - 1) + (float) (num_of_iterations % 2) * 15));
}