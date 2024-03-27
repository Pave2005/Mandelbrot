#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <xmmintrin.h>

#include "Mandelbrot.h"

int main ()
{
    RenderWindow window (VideoMode (width, height), "Mandelbrot Set");

    Image image;
    image.create (width, height, Color::Black);

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
        }

        DrawMaldelbrotSmpVers (image);

        texture.update (image);

        window.clear ();
        window.draw (sprite);
        window.display ();
    }
}

