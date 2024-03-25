#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdio.h>
#include <raylib.h>

int Max(int a, int b);

void DrawCornerDimensions(Vector2 topLeft, Vector2 bottomRight);
void DrawLineStripFromPoints(Vector2 *points, Vector2 offset, float scale, size_t size);

#endif // UTILITIES_H
