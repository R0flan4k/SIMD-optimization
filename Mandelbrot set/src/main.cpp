#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>

#include "Mandelbrot_set.h"
#include "flags.h"
#include "cmd_input.h"

int main(int argc, char * argv[])
{
    if (!check_cmd_input(argc, argv))       // Cmd line args handling.
    {
        return 1;
    }

    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HIGHT), "Mandelbrot set");
    sf::VertexArray set(sf::Points, SCREEN_HIGHT * SCREEN_WIDTH);
    sf::Clock clock;
    sf::Time time;
    sf::Text framerate;
    sf::Font font;
    char framerate_str[16] = "";

    framerate_text_ctor(&framerate, &font, "./Font/CyrilicOld.TTF");

    PixelCs cs_centre = {.cs_x = SCREEN_WIDTH * 0.6f,   // Offset the coordinates origin so that the figure is centered.
                         .cs_y = SCREEN_HIGHT / 2};
    float scale = 0.003f;                               // Screen scale.

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            cs_centre.cs_y += ELEM_SCREEN_OFFSET; 
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            cs_centre.cs_x += ELEM_SCREEN_OFFSET; 
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            cs_centre.cs_y -= ELEM_SCREEN_OFFSET; 
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            cs_centre.cs_x -= ELEM_SCREEN_OFFSET;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
        {
            scale /= SCALE_COEF;
            cs_centre.cs_x += (SCALE_COEF - 1.f) * (cs_centre.cs_x - SCREEN_WIDTH / 2);      // To save center position.
            cs_centre.cs_y += (SCALE_COEF - 1.f) * (cs_centre.cs_y - SCREEN_HIGHT / 2);      // (Hard math calculations)
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
        {
            scale *= 1.1f;
            cs_centre.cs_x -= (SCALE_COEF - 1.f) / SCALE_COEF * (cs_centre.cs_x - SCREEN_WIDTH / 2);  // To save center position.
            cs_centre.cs_y -= (SCALE_COEF - 1.f) / SCALE_COEF * (cs_centre.cs_y - SCREEN_HIGHT / 2);  // (Hard math calculations)
        }

        clock.restart();                    
        DRAW_FUNCTION(&set, cs_centre, scale);
        time = clock.getElapsedTime();     
        sprintf(framerate_str, "%.2f", 1 / time.asSeconds());
        framerate.setString(framerate_str);

        window.clear();
        window.draw(set);
        window.draw(framerate);
        window.display();
    }

    return 0;
}