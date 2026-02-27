#include "graphics.h"
#include "include/raymath.h"

void Graphics_DrawFormulaCar(Vector2 pos, float rotation, Color carColor) {
    DrawRectanglePro((Rectangle){pos.x, pos.y, 14, 55}, (Vector2){7, 27}, rotation, carColor);
    DrawRectanglePro((Rectangle){pos.x, pos.y, 38, 8}, (Vector2){19, 27}, rotation, Fade(carColor, 0.8f));
    DrawRectanglePro((Rectangle){pos.x, pos.y, 42, 3}, (Vector2){21, 27}, rotation, BLACK);
    DrawRectanglePro((Rectangle){pos.x, pos.y, 34, 10}, (Vector2){17, -18}, rotation, DARKGRAY);
    DrawRectanglePro((Rectangle){pos.x, pos.y, 34, 4}, (Vector2){17, -20}, rotation, BLACK);
    DrawRectanglePro((Rectangle){pos.x, pos.y, 10, 18}, (Vector2){24, 15}, rotation, BLACK);
    DrawRectanglePro((Rectangle){pos.x, pos.y, 10, 18}, (Vector2){-14, 15}, rotation, BLACK);
    DrawRectanglePro((Rectangle){pos.x, pos.y, 12, 20}, (Vector2){25, -8}, rotation, BLACK);
    DrawRectanglePro((Rectangle){pos.x, pos.y, 12, 20}, (Vector2){-13, -8}, rotation, BLACK);
    DrawRectanglePro((Rectangle){pos.x, pos.y, 8, 8}, (Vector2){4, 0}, rotation, YELLOW);
    DrawRectanglePro((Rectangle){pos.x, pos.y, 12, 4}, (Vector2){6, 5}, rotation, BLACK);
}

void Graphics_DrawHeadlights(Vector2 pos, float rotation, int weatherType) {
    if (weatherType == 2 || weatherType == 3) { // NIGHT or RAINY
        BeginBlendMode(BLEND_ADDITIVE);
            Vector2 leftH = Vector2Add(pos, Vector2Rotate((Vector2){-10, -25}, rotation * DEG2RAD));
            Vector2 rightH = Vector2Add(pos, Vector2Rotate((Vector2){10, -25}, rotation * DEG2RAD));
            Vector2 lightDir = Vector2Rotate((Vector2){0, -500}, rotation * DEG2RAD);
            DrawTriangle(leftH, Vector2Add(leftH, Vector2Add(lightDir, Vector2Rotate((Vector2){-80,0}, rotation*DEG2RAD))), 
                Vector2Add(leftH, Vector2Add(lightDir, Vector2Rotate((Vector2){80,0}, rotation*DEG2RAD))), Fade(WHITE, 0.15f));
            DrawTriangle(rightH, Vector2Add(rightH, Vector2Add(lightDir, Vector2Rotate((Vector2){-80,0}, rotation*DEG2RAD))), 
                Vector2Add(rightH, Vector2Add(lightDir, Vector2Rotate((Vector2){80,0}, rotation*DEG2RAD))), Fade(WHITE, 0.15f));
        EndBlendMode();
    }
}
