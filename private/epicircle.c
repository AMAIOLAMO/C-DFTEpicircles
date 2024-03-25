#include <raylib.h>
#include <raymath.h>

#include "../include/epicircle.h"

Vector2 GetVectorRotated(float radius, float angle) {
  return (Vector2){radius * cos(angle), radius * sin(angle)};
}

Epicircle CreateEpicircle(float freq, float amp, float phase) {
  return (Epicircle){freq, amp, phase};
}

Vector2 EpicircleEndPoint(Epicircle epicircle, float time) {
  float freq = epicircle.freq;
  float amp = epicircle.amp;
  float phase = epicircle.phase;

  float angle = time * freq + phase;

  return GetVectorRotated(amp, angle);
}

Vector2 EpicirclesEndPoint(Epicircle *epicircles, float time,
                                size_t size) {
  Vector2 endPoint = {0};

  for (int i = 0; i < size; i++) {
    Vector2 endPointOffset = EpicircleEndPoint(epicircles[i], time);
    endPoint = Vector2Add(endPoint, endPointOffset);
  }

  return endPoint;
}

Vector2 DrawEpicircle(Epicircle epicircle, Vector2 origin, float time,
                      Color color, float angleOffset) {
  float freq = epicircle.freq;
  float amp = epicircle.amp;
  float phase = epicircle.phase;
  float angle = time * freq + phase;

  DrawCircleLinesV(origin, amp, color);

  Vector2 rotatedEnd = GetVectorRotated(amp, angle + angleOffset);

  Vector2 endPoint = Vector2Add(origin, rotatedEnd);

  DrawLineV(origin, endPoint, color);

  return endPoint;
}

Vector2 DrawEpicircles(Epicircle *epicircles, Vector2 origin, float time,
                       Color color, size_t size, float angleOffset) {
  Vector2 currentOrigin = origin;

  for (int i = 0; i < size; i++) {
    Epicircle epicircle = epicircles[i];

    currentOrigin = DrawEpicircle(epicircle, currentOrigin, time, color, angleOffset);
  }

  return currentOrigin;
}
