
#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"



void testSvg()
{
    // Load
    struct NSVGimage* image;
    image = nsvgParseFromFile("test.svg", "px", 96);
    printf("size: %f x %f\n", image->width, image->height);
    // Use...

    // Delete
    nsvgDelete(image);
}
