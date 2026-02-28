#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "external/raylib/include/raylib.h"

#define TOTAL_COLORS 6

extern Color carColors[TOTAL_COLORS];

void Graphics_DrawFormulaCar(Vector2 pos, float rotation, Color carColor);
void Graphics_DrawHeadlights(Vector2 pos, float rotation, int weatherType);

#endif
