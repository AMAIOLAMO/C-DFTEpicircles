#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/raylib.h"
#include "raylib.h"
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "include/epicircle.h"
#include "include/raygui.h"

#include "include/fouriers.h"

#define ARRAY_LEN(a) (sizeof(a) / sizeof(a[0]))

int Max(int a, int b) {
  if(a > b) {
    return a;
  }

  return b;
}

// reads until found a delimiter of , or \n, and stops at the index where the delimiter is found
// which means you need to manually increment the read ptr in order to read the next
bool ReadNextFloat(char *text, int *textReadPtr, float *resultPtr, size_t textLength) {
  if(*textReadPtr >= textLength)
    return false;

  if(text[*textReadPtr] == ',' || text[*textReadPtr] == '\n')
    return false;

  char readBuffer[100] = {};
  int readBufferWriteIndex = 0;

  while(*textReadPtr < textLength) {
    char character = text[*textReadPtr];

    if (character == ',' || character == '\n') {

      // add terminating character so it doesnt read too long
      readBuffer[readBufferWriteIndex] = '\0';

      *resultPtr = strtof(&readBuffer[0], NULL);

      return true;
    }
    // else

    readBuffer[readBufferWriteIndex++] = character;

    *textReadPtr += 1;
  }

  return false;
}

int LoadDiscreteValuesFromFile(const char *fileName, Vector2 **discreteValuesPtr) {
  char *fileText = LoadFileText(fileName);

  int fileLength = GetFileLength(fileName);

  int textReadIndex = 0;
  int valuesLength = 0;

  *discreteValuesPtr = (Vector2*)malloc(2000 * sizeof(Vector2));

  while(textReadIndex < fileLength && valuesLength <= 2000) {
    Vector2 discreteValue = {};

    ReadNextFloat(fileText, &textReadIndex, &discreteValue.x, fileLength);
    textReadIndex += 1;

    ReadNextFloat(fileText, &textReadIndex, &discreteValue.y, fileLength);
    textReadIndex += 1;

    (*discreteValuesPtr)[Max(valuesLength - 1, 0)] = discreteValue;
    valuesLength += 1;
  }

  UnloadFileText(fileText);

  return valuesLength;
}

void UnloadDiscreteValues(Vector2 *discreteValues) {
  free(discreteValues);
}

int main(void) {
  InitWindow(1000, 900, "Discrete Fourier Transform");
  SetTargetFPS(120);

  const char *FILE_NAME = "sampled_svgs/cute_cats.pt";

  Vector2 *discreteValues;
  int valuesLength = LoadDiscreteValuesFromFile(FILE_NAME, &discreteValues);

  printf("[Discrete value loader] loaded from file: %s, of total %d discrete values", FILE_NAME, valuesLength);

  Epicircle *xEpicircles = (Epicircle*)malloc(valuesLength * sizeof(Epicircle));
  Epicircle *yEpicircles = (Epicircle*)malloc(valuesLength * sizeof(Epicircle));

  DFourierTransform2D(discreteValues, xEpicircles, yEpicircles, valuesLength);

  float time = 0.0f;

  // create values
  // create corresponding epicircles
  // draw epicircles depending on time

  Vector2 points[1000] = {};

  int pointsIndex = 0;
  const int pointsLength = ARRAY_LEN(points);

  int totalFrames = 0;

  bool displayFps = true;

  Vector2 drawingOffset = {GetScreenWidth() * .5f, GetScreenWidth() * .5f};
  float drawingScale = 1.0f;

  while (!WindowShouldClose()) {
    BeginDrawing();

    {
      ClearBackground(BLACK);

      if (displayFps)
        DrawFPS(GetScreenWidth() - 100, 0);

      {
        Rectangle rect = {0, 0, 300, 20};
        GuiSlider(rect, "", "offset x", &drawingOffset.x, 0, GetScreenWidth());
      }

      {
        Rectangle rect = {0, 30, 300, 20};
        GuiSlider(rect, "", "offset y", &drawingOffset.y, 0, GetScreenHeight());
      }

      {
        Rectangle rect = {0, 60, 300, 20};
        GuiSlider(rect, "", "scale", &drawingScale, 0, 10);
      }

      // uses totalFrames due to the floating point error that exists after
      // running a few minutes, which distorts the epicircles
      time = totalFrames * ((2.0f * PI) / valuesLength);
      // time += (2.0f * PI) / size;
      totalFrames += 1;

      {
        // draw left epicircles which draws the X axis of the drawing
        {
          Vector2 yOrigin = {GetScreenWidth() * .5f - 200.0f,
                             GetScreenHeight() * .5f};

          Vector2 yEpicirclesEndPoint =
              DrawEpicircles(yEpicircles, yOrigin, time, WHITE, valuesLength, PI / 2);

          // draws end point
          DrawCircleV(yEpicirclesEndPoint, 5.0f, GREEN);
        }

        // draws top epicircles which draws the X axis of the drawing
        {
          Vector2 xOrigin = {GetScreenWidth() * .5f,
                             GetScreenHeight() * .5f - 200.0f};

          Vector2 xEpicirclesEndPoint =
              DrawEpicircles(xEpicircles, xOrigin, time, WHITE, valuesLength, 0);

          // draws end point
          DrawCircleV(xEpicirclesEndPoint, 5.0f, GREEN);
        }

        Vector2 xEndPoint = GetEndPointOfEpicircles(xEpicircles, time, valuesLength);

        float xRealPart = xEndPoint.x;

        Vector2 yEndPoint = GetEndPointOfEpicircles(yEpicircles, time, valuesLength);

        float yRealPart = yEndPoint.x;

        // shifts array elements to the right and appends element at the start

        // size_t numberOfBytesToCopy = (ARRAY_LEN(ys) - 1) * sizeof(ys[0]);
        size_t numberOfBytesToCopy = (pointsLength - 1) * sizeof(points[0]);

        // memmove(&ys[1], &ys[0], numberOfBytesToCopy);
        memmove(&points[1], &points[0], numberOfBytesToCopy);

        points[0] = (Vector2){xRealPart, yRealPart};

        if (pointsIndex < pointsLength)
          pointsIndex++;
      }

      // we could potentially optimize using an image
      // live display while drawing the image

      for (int i = 0; i < pointsIndex - 1; i++) {
        Vector2 scaledStartPoint = Vector2Scale(points[i], drawingScale);
        Vector2 scaledEndPoint = Vector2Scale(points[i + 1], drawingScale);

        Vector2 startPoint = Vector2Add(scaledStartPoint, drawingOffset);
        Vector2 endPoint = Vector2Add(scaledEndPoint, drawingOffset);

        const float percentage = (float)i / (float)pointsLength;

        const Color test =
            ColorFromHSV(percentage * 255.0f, 1.0f, 1.0f - percentage);

        DrawLineEx(startPoint, endPoint, 3.0f, test);
      }

      char content[100] = {0};

      sprintf(content, "index: %d", pointsIndex);
      DrawText(content, 0, 100, 20, WHITE);
    }

    EndDrawing();
  }

  UnloadDiscreteValues(discreteValues);
  free(xEpicircles);
  free(yEpicircles);
  return 0;
}
