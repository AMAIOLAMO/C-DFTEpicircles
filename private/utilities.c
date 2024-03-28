#include <stdio.h>
#include <raylib.h>
#include <raymath.h>

int MaxI(int a, int b) {
  if(a > b) {
    return a;
  }

  return b;
}

float Maxf(float a, float b) {
  if(a > b) {
    return a;
  }

  return b;
}

void DrawCornerDimensions(Vector2 topLeft, Vector2 bottomRight) {
  DrawLineV(topLeft, Vector2Add(topLeft, (Vector2){50.0f, 0.0f}), GREEN);
  DrawLineV(topLeft, Vector2Add(topLeft, (Vector2){0.0f, 50.0f}), GREEN);

  DrawLineV(bottomRight, Vector2Add(bottomRight, (Vector2){-50.0f, 0.0f}), GREEN);
  DrawLineV(bottomRight, Vector2Add(bottomRight, (Vector2){0.0f, -50.0f}), GREEN);
}

void DrawLineStripFromPoints(Vector2 *points, Vector2 offset, float scale, float ignoreDistanceThreshold, float lineThickness, size_t size) {
  for (int i = 0; i < size - 1; i++) {
    if(Vector2DistanceSqr(points[i], points[i + 1]) > ignoreDistanceThreshold * ignoreDistanceThreshold)
      continue;

    Vector2 scaledStartPoint = Vector2Scale(points[i], scale);
    Vector2 scaledEndPoint = Vector2Scale(points[i + 1], scale);

    Vector2 startPoint = Vector2Add(scaledStartPoint, offset);
    Vector2 endPoint = Vector2Add(scaledEndPoint, offset);

    const float percentage = (float)i / (float)size;

    const Color color =
      ColorFromHSV(percentage * 255.0f, 1.0f, 1.0f);

    DrawLineEx(startPoint, endPoint, lineThickness, color);
  }
}
