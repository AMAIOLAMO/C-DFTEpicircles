#pragma once

#include <stdio.h>
#include <raylib.h>

struct Vector2;

typedef struct Epicircle {
  float freq, amp, phase;
} Epicircle;

Epicircle CreateEpicircle(float freq, float amp, float phase);

// endpoint calculation
struct Vector2 GetEndPointOfEpicircle(Epicircle epicircle, float time);
struct Vector2 GetEndPointOfEpicircles(Epicircle *epicircles, float time, size_t size);

// drawing
Vector2 DrawEpicircle(Epicircle epicircle, Vector2 origin, float time, Color color, float angleOffset);
Vector2 DrawEpicircles(Epicircle *epicircles, Vector2 origin, float time, Color color, size_t size, float angleOffset);
