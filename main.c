#define RAYGUI_IMPLEMENTATION

#include "include/raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <raylib.h>
#include <raymath.h>

#include "include/epicircle.h"
#include "include/raygui.h"
#include "include/utilities.h"

#include "include/fouriers.h"

#define ARRAY_LEN(a) (sizeof(a) / sizeof(a[0]))

void UpdateCameraMovement(Camera2D *camera, float baseCameraSpeed) {
  float finalCameraSpeed = baseCameraSpeed;

  if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
    finalCameraSpeed *= 2.0f;
  }

  if (IsKeyDown(KEY_W)) {
    camera->target.y -= finalCameraSpeed * GetFrameTime();
  }
  if (IsKeyDown(KEY_S)) {
    camera->target.y += finalCameraSpeed * GetFrameTime();
  }
  if (IsKeyDown(KEY_A)) {
    camera->target.x -= finalCameraSpeed * GetFrameTime();
  }
  if (IsKeyDown(KEY_D)) {
    camera->target.x += finalCameraSpeed * GetFrameTime();
  }
}

void UpdateCameraZoom(Camera2D *camera, float zoomIncreaseFactor,
                      float zoomDecreaseFactor) {
  if (IsKeyPressed(KEY_EQUAL)) {
    camera->zoom += zoomIncreaseFactor;
  }
  if (IsKeyPressed(KEY_MINUS)) {
    camera->zoom = Maxf(camera->zoom - zoomDecreaseFactor, 0.1f);
  }
}

void DrawGrid2D(int gridSize, int tileSize, Color color) {
  const int tile_size = 50;
  for (int i = -gridSize; i < gridSize; i++) {
    DrawLineEx((Vector2){gridSize * tileSize, tileSize * i},
               (Vector2){-gridSize * tileSize, tileSize * i}, 1, color);

    DrawLineEx((Vector2){tileSize * i, gridSize * tileSize},
               (Vector2){tileSize * i, -gridSize * tileSize}, 1, color);
  }
}

void DrawAxis(int gridSize, int tileSize, Color color) {
  DrawLineEx((Vector2){gridSize * tileSize, 0},
             (Vector2){-gridSize * tileSize, 0}, 2, color);
  DrawLineEx((Vector2){0, gridSize * tileSize},
             (Vector2){0, -gridSize * tileSize}, 2, color);
  for (int i = -gridSize; i < gridSize; i++) {
    DrawText(TextFormat("%d", i * tileSize), i * tileSize + 5, 5, 5, color);
    DrawText(TextFormat("%d", i * tileSize), 5, i * tileSize + 5, 5, color);
  }
}

// reads until found a delimiter of , or \n, and stops at the index where the
// delimiter is found which means you need to manually increment the read ptr in
// order to read the next
bool ReadNextFloat(char *text, int *textReadPtr, float *resultPtr,
                   size_t textLength) {
  if (*textReadPtr >= textLength)
    return false;

  if (text[*textReadPtr] == ',' || text[*textReadPtr] == '\n')
    return false;

  char readBuffer[100] = {};
  int readBufferWriteIndex = 0;

  while (*textReadPtr < textLength) {
    char character = text[*textReadPtr];

    if (character == ',' || character == '\n') {

      // add terminating character so it doesnt read too long
      readBuffer[readBufferWriteIndex] = '\0';

      *resultPtr = TextToFloat(&readBuffer[0]);

      return true;
    }
    // else

    readBuffer[readBufferWriteIndex++] = character;

    *textReadPtr += 1;
  }

  return false;
}

int LoadDiscreteValuesFromFile(const char *fileName,
                               Vector2 **discreteValuesPtr) {
  char *fileText = LoadFileText(fileName);

  int fileLength = GetFileLength(fileName);

  int textReadIndex = 0;
  int valuesLength = 0;

  *discreteValuesPtr = (Vector2 *)malloc(2000 * sizeof(Vector2));

  while (textReadIndex < fileLength && valuesLength <= 2000) {
    Vector2 discreteValue = {};

    ReadNextFloat(fileText, &textReadIndex, &discreteValue.x, fileLength);
    textReadIndex += 1;

    ReadNextFloat(fileText, &textReadIndex, &discreteValue.y, fileLength);
    textReadIndex += 1;

    // append to the end of the array
    (*discreteValuesPtr)[MaxI(valuesLength - 1, 0)] = discreteValue;
    valuesLength += 1;
  }

  UnloadFileText(fileText);

  return valuesLength;
}

void UnloadDiscreteValues(Vector2 *discreteValues) { free(discreteValues); }

int main(void) {
  InitWindow(1000, 900, "Discrete Fourier Transform");
  SetTargetFPS(120);

  const char *FILE_NAME = "sampled_svgs/love_text.pt";

  Vector2 maxDrawDimension;

  Vector2 *discreteValues;
  int valuesLength = LoadDiscreteValuesFromFile(FILE_NAME, &discreteValues);

  printf("[Discrete value loader] loaded from file: %s, of total %d discrete "
         "values\n",
         FILE_NAME, valuesLength);

  for (size_t i = 0; i < valuesLength; i++) {
    Vector2 value = discreteValues[i];

    maxDrawDimension = Vector2Max(maxDrawDimension, value);
  }

  printf("max dimension: <%f, %f>\n", maxDrawDimension.x, maxDrawDimension.y);

  Epicircle *xEpicircles =
      (Epicircle *)malloc(valuesLength * sizeof(Epicircle));
  Epicircle *yEpicircles =
      (Epicircle *)malloc(valuesLength * sizeof(Epicircle));

  DFourierTransform2D(discreteValues, xEpicircles, yEpicircles, valuesLength);

  float time = 0.0f;

  // create values
  // create corresponding epicircles
  // draw epicircles depending on time

  Vector2 displayPointBuffer[1000] = {};

  int currentPointsLength = 0;
  const int pointsAbsoluteLength = ARRAY_LEN(displayPointBuffer);

  int totalFrames = 0;

  bool displayFps = true;

  Vector2 drawingOrigin = Vector2Zero();

  float drawingScale = 1.0f;

  float ignoreDistanceThreshold = 200.0f;

  Camera2D camera;
  camera.offset = (Vector2){GetScreenWidth() * .5f, GetScreenWidth() * .5f};
  camera.target = Vector2Zero();
  camera.rotation = 0;
  camera.zoom = 1.0f;

  float baseCameraSpeed = 300.0f;

  while (!WindowShouldClose()) {
    BeginDrawing();

    {
      ClearBackground(BLACK);

      UpdateCameraMovement(&camera, baseCameraSpeed);
      UpdateCameraZoom(&camera, 0.3f, 0.3f);

      BeginMode2D(camera);

      DrawGrid2D(100, 50, ColorFromHSV(0.0f, 0.0f, 0.2f));
      DrawAxis(100, 50, ColorFromHSV(50, 0.5f, 1.0f));

      // uses totalFrames due to the floating point error that exists after
      // running a few minutes, which distorts the epicircles
      time = totalFrames * ((2.0f * PI) / valuesLength);
      // time += (2.0f * PI) / size;
      totalFrames += 1;

      Vector2 scaledMaxDrawDimension =
          Vector2Scale(maxDrawDimension, drawingScale);
      Vector2 halfScaledMaxDimension =
          Vector2Scale(scaledMaxDrawDimension, 0.5f);

      Vector2 topLeft = Vector2Subtract(drawingOrigin, halfScaledMaxDimension);
      Vector2 bottomRight = Vector2Add(halfScaledMaxDimension, drawingOrigin);

      DrawCornerDimensions(topLeft, bottomRight);

      {
        Vector2 xEndPoint =
            EpicirclesEndPoint(xEpicircles, time, valuesLength);

        float xRealPart = xEndPoint.x;

        Vector2 yEndPoint =
            EpicirclesEndPoint(yEpicircles, time, valuesLength);

        float yRealPart = yEndPoint.x;

        Vector2 resultVectorPoint = Vector2Subtract((Vector2){xRealPart, yRealPart}, halfScaledMaxDimension);
        resultVectorPoint = Vector2Add(drawingOrigin, resultVectorPoint);

        DrawLineV(drawingOrigin, resultVectorPoint, YELLOW);


        // draw left epicircles which draws the Y axis of the drawing
        {
          Vector2 leftEpicircleOrigin = topLeft;
          leftEpicircleOrigin = Vector2Subtract(leftEpicircleOrigin, (Vector2){100.0f, 0.0f});

          Vector2 yEpicirclesEndPoint = DrawEpicircles(
              yEpicircles, leftEpicircleOrigin, time, GRAY, valuesLength, PI / 2);

          // draw center
          DrawCircleLinesV(leftEpicircleOrigin, 5.0f, RED);

          // draws end point
          DrawCircleV(yEpicirclesEndPoint, 5.0f, GREEN);

          DrawLineV(yEpicirclesEndPoint, resultVectorPoint, GRAY);
        }

        // draws top epicircles which draws the X axis of the drawing
        {
          Vector2 topEpicircleOrigin = topLeft;
          topEpicircleOrigin = Vector2Subtract(topEpicircleOrigin, (Vector2){0.0f, 100.0f});

          Vector2 xEpicirclesEndPoint =
              DrawEpicircles(xEpicircles, topEpicircleOrigin, time, GRAY, valuesLength, 0);

          // draw center
          DrawCircleLinesV(topEpicircleOrigin, 5.0f, RED);

          // draws end point
          DrawCircleV(xEpicirclesEndPoint, 5.0f, GREEN);

          DrawLineV(xEpicirclesEndPoint, resultVectorPoint, GRAY);
        }

        // shifts array elements to the right and appends element at the start
        size_t numberOfBytesToCopy =
            (pointsAbsoluteLength - 1) * sizeof(displayPointBuffer[0]);

        memmove(&displayPointBuffer[1], &displayPointBuffer[0],
                numberOfBytesToCopy);

        displayPointBuffer[0] = (Vector2){xRealPart, yRealPart};

        if (currentPointsLength < pointsAbsoluteLength)
          currentPointsLength++;
      }

      // we could potentially optimize using an image
      // live display while drawing the image
      DrawLineStripFromPoints(displayPointBuffer, topLeft, drawingScale,
                              ignoreDistanceThreshold, currentPointsLength);
    }

    EndMode2D();

    DrawText(TextFormat("index: %d", currentPointsLength), 0, 110, 20, WHITE);

    // UI related stuff

    if (displayFps)
      DrawFPS(GetScreenWidth() - 100, 0);

    {
      Rectangle rect = {0, 0, 300, 20};
      GuiSlider(rect, "", "origin x", &drawingOrigin.x, -GetScreenWidth(), GetScreenWidth());
    }

    {
      Rectangle rect = {0, 30, 300, 20};
      GuiSlider(rect, "", "origin y", &drawingOrigin.y, -GetScreenHeight(), GetScreenHeight());
    }

    {
      Rectangle rect = {0, 60, 300, 20};
      GuiSlider(rect, "", "scale", &drawingScale, 0, 10);
    }

    {
      Rectangle rect = {0, 90, 300, 20};
      GuiSlider(rect, "", "ignore Distance Threshold", &ignoreDistanceThreshold,
                0, 500);
    }

    EndDrawing();
  }

  UnloadDiscreteValues(discreteValues);

  free(xEpicircles);
  free(yEpicircles);
  return 0;
}
