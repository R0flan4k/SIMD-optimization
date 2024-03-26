#include <stdio.h>
#include <string.h>

#include "cmd_input.h"
#include "my_assert.h"
#include "flags.h"
#include "Mandelbrot_set.h"

void (*DRAW_FUNCTION)(sf::VertexArray * set, const PixelCs cs_centre,
                             const float scale) = simd_mandelbrot_draw;
char * * cmd_input = NULL;

CmdLineArg NO_SIMD_OPTIMIZATION = {
    .name =          "--no-simd",
    .num_of_param =  0,
    .flag_function = set_default_calculations,
    .argc_number =   0,
    .help =          "--no-simd",
    .is_mandatory = true,
};

CmdLineArg * FLAGS[] = {&NO_SIMD_OPTIMIZATION};
size_t FLAGS_ARRAY_SIZE = sizeof(FLAGS) / sizeof(FLAGS[0]);


void show_error_message(const char * program_name)
{
    printf("Error. Please, use %s %s\n", program_name, NO_SIMD_OPTIMIZATION.help);
}

void set_default_calculations(void)
{
    DRAW_FUNCTION = default_mandelbrot_draw;
}