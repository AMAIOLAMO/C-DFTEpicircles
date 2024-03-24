#include <raylib.h>
#include <raymath.h>
#include <math.h>

#include "../include/epicircle.h"
#include "../include/fouriers.h"

void DFourierTranform(float *input, Epicircle *epicircles, size_t size) {

  for (int k = 0; k < size; k++) {
    float re = 0.0f;
    float im = 0.0f;

    for (int n = 0; n < size; n++) {
      float angle = (2.0f * PI * k * n) / size;

      re += input[n] * cosf(angle);
      im -= input[n] * sinf(angle);
    }

    re /= size;
    im /= size;

    float amp = sqrtf(re * re + im * im);
    float phase = atan2f(im, re);

    epicircles[k] = CreateEpicircle(k, amp, phase);
  }
}

void DFourierTransform2D(Vector2 *input, Epicircle *xEpicircles,
                         Epicircle *yEpicircles, size_t size) {
  for (int k = 0; k < size; k++) {
    Vector2 re = {0.0f, 0.0f};
    Vector2 im = {0.0f, 0.0f};

    for (int n = 0; n < size; n++) {
      float angle = (2.0f * PI * k * n) / size;

      // re += input[n] * cosf(angle);
      re = Vector2Add(re, Vector2Scale(input[n], cosf(angle)));
      // im -= input[n] * sinf(angle);
      im = Vector2Subtract(im, Vector2Scale(input[n], sinf(angle)));
    }

    // re /= size;
    // im /= size;

    re = Vector2Scale(re, 1.0f / size);
    im = Vector2Scale(im, 1.0f / size);

    // float amp = sqrtf(re * re + im * im);
    // float phase = atan2f(im, re);
    Vector2 amp = {sqrtf(re.x * re.x + im.x * im.x),
                   sqrtf(re.y * re.y + im.y * im.y)};

    Vector2 phase = {atan2(im.x, re.x), atan2(im.y, re.y)};

    // epicircles[k] = CreateEpicircle(k, amp, phase);

    xEpicircles[k] = CreateEpicircle(k, amp.x, phase.x);
    yEpicircles[k] = CreateEpicircle(k, amp.y, phase.y);
  }
}
