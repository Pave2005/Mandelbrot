#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <xmmintrin.h>
#include <stdlib.h>

#include "Mandelbrot.h"

int main ()
{
    #ifndef TEST
    RenderWindow window (VideoMode (width, height), "Mandelbrot Set");
    #endif

    Image image;
    float scale = 3;
    float users_shift = 0;

    #ifndef TEST
    image.create (width, height, Color::White);

    Texture texture;
    texture.create (width, height);
    Sprite sprite (texture);

    while (window.isOpen ())
    {
        Event event;
        while (window.pollEvent (event))
        {
            if (event.type == Event::Closed)
                window.close ();

            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Up) scale += 1;

                else if (event.key.code == Keyboard::Down) scale -= 1;

                else if (event.key.code == Keyboard::Right) users_shift += 50;

                else if (event.key.code == Keyboard::Left) users_shift -= 50;
            }
        }
    #endif

        clock_t start = clock ();
        DrawMaldelbrotMidVers (&image, scale, users_shift);
        clock_t finish = clock ();

        int fps_num = FpsCounter (start, finish);

        char fps_str[10] = {};
        sprintf (fps_str, "%d", fps_num);

    #ifndef TEST
        Font font;
        font.loadFromFile("times.ttf");
        Text text("", font, 20);

        text.setString(fps_str);
		text.setPosition(0, 0);

        texture.update (image);
        window.clear ();
        window.draw (sprite);
        window.draw(text);
        window.display ();
    }
    #endif
}

