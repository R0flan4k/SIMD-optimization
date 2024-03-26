#ifndef FLAGS_H
    #define FLAGS_H

    #include "cmd_input.h"
    #include "Mandelbrot_set.h"

    extern CmdLineArg NO_SIMD_OPTIMIZATION;
    extern void (*DRAW_FUNCTION)(sf::VertexArray * set, const PixelCs cs_centre,
                                        const float scale);

    extern char * * cmd_input;
    extern CmdLineArg * FLAGS[];
    extern size_t FLAGS_ARRAY_SIZE;

    void show_error_message(const char * program_name);
    void set_default_calculations(void);

#endif // FLAGS_H