#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <o2/time.h>
#include <string.h>
#include <o2/arena.h>
#include <o2/world.h>
#include <assert.h>
#include <o2/raylib/display.h>
#include <o2/math.h>
#include <o2/raylib/asset.h>

void printError(const char* message, const char* file, int lineNumber);
void simpleNumToString(char* output, int num, int maxDigits);

int main() {
	int returnCode = EXIT_SUCCESS;
	
	Texture wabbit = { 0 };
	Texture tilesTexture = { 0 };

	size_t heapSize = 4 * MEGABYTE;
	uint8* memory = (uint8*)malloc(heapSize);
	o2_Arena globalArena = {
		.base = memory,
		.size = heapSize,
		.used = 0
	};

	{
		if (NULL == memory) {
			printError("Failed to allocate heap memory", __FILE__, __LINE__);
			returnCode = EXIT_FAILURE;
			goto CLEAN_RESOURCES;
		}

		SetConfigFlags(FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_RESIZABLE);

		int monitorId = GetCurrentMonitor();
		int width = GetMonitorWidth(monitorId);
		int height = GetMonitorHeight(monitorId);

		int screenWidth = 1280;
		int screenHeight = 720;
		InitWindow(screenWidth, screenHeight, "Hello Raylib");
		if (!IsWindowReady()) {
			printError("Failed to initialize window", __FILE__, __LINE__);
			returnCode = EXIT_FAILURE;
			goto CLEAN_RESOURCES;
		}
		SetExitKey(KEY_NULL);

		// Load a texture from the resources directory
		wabbit = LoadTexture("resources/wabbit_alpha.png");
		if (!IsTextureValid(wabbit)) {
			printError("Failed to load texture: resources/wabbit_alpha.png", __FILE__, __LINE__);
			returnCode = EXIT_FAILURE;
			goto CLEAN_RESOURCES;
		}

		tilesTexture = LoadTexture("resources/tile_sheet.png");
		if (!IsTextureValid(tilesTexture)) {
			printError("Failed to load texture: resources/tile_sheet.png", __FILE__, __LINE__);
			returnCode = EXIT_FAILURE;
			goto CLEAN_RESOURCES;
		}
		int tileWidth = 32;
		int tileHeight = 32;
		SpriteSheet tileSpriteSheet = o2_asset_createUniformSpriteSheet(&globalArena, tilesTexture, tileWidth, tileHeight);


		Camera2D worldCamera = {
			.offset = (Vector2){width / 2.0, height / 2.0},
			.target = (Vector2){0, 0},
			.rotation = 0,
			.zoom = 1.f
		};
		int worldCameraSpeed = 200;
		float zoomLevels[3] = { 0.5f, 1.0f, 1.5f };
		int maxZoomLevel = 2;
		int currentZoomLevel = 1;
		//worldCamera.target = (Vector2){ -8000, -8000 };

		int basePanelHeight = 224;
		BasePanel basePanel;
		o2_display_initBasePanel(&basePanel, (Rectangle) { 0, screenHeight - basePanelHeight, screenWidth, basePanelHeight });

		char fpsDigits[8];
		const int maxFpsDigits = 7;
		memset(fpsDigits, '\0', 8);
		simpleNumToString(fpsDigits, 0, maxFpsDigits);

		int fpsTarget = 240;
		o2_Limiter fpsLimiter = {
			.threshold = 1.0 / fpsTarget,
			.last = 0
		};

		o2_Limiter movementLimiter = {
			.threshold = 1.0 / fpsTarget,
			.last = 0
		};

		o2_Limiter mouseInputLimiter = {
			.threshold = 1.0 / fpsTarget,
			.last = 0
		};

		int rowsOfChunks = 32;
		int colsOfChunks = 32;
		uint8 chunkRows = 16;
		uint8 chunkCols = 16;
		int chunkWidth = chunkCols * tileWidth;
		int chunkHeight = chunkRows * tileHeight;
		int minChunkX = 0 - colsOfChunks / 2;
		int minChunkY = 0 - rowsOfChunks / 2;
		int maxChunkX = minChunkX + colsOfChunks;
		int maxChunkY = minChunkY + rowsOfChunks;
		o2_WorldSettings worldSettings = {
			.rowsOfChunks = 32,
			.colsOfChunks = 32,
			.rowsOfTiles = 16,
			.colsOfTiles = 16,
			.tileWidth = tileWidth,
			.tileHeight = tileHeight
		};
		o2_World world = o2_world_createWorld(&globalArena, worldSettings);

		printf("Min chunk coords: %d %d\n", minChunkX, minChunkY);
		printf("Max chunk coords: %d %d\n", maxChunkX, maxChunkY);
		printf("Pre game loop arena usage: %llu\n", globalArena.used);

		// game loop
		while (!WindowShouldClose()) {
			double currentTime = GetTime();

			if (IsWindowResized()) {
				screenWidth = GetScreenWidth();
				screenHeight = GetScreenHeight();
			}

			if (o2_time_limiterResetIfReady(&mouseInputLimiter, currentTime)) {
				float mouseWheel = GetMouseWheelMove();
				if (mouseWheel != 0) {
					printf("Mouse Wheel %g\n", mouseWheel);
					if (mouseWheel > 0) {
						currentZoomLevel = o2_math_imax(0, currentZoomLevel - 1);
					}
					else {
						currentZoomLevel = o2_math_imin(maxZoomLevel, currentZoomLevel + 1);
					}
					worldCamera.zoom = zoomLevels[currentZoomLevel];
				}

				if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {

				}
				if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
					Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), worldCamera);
					o2_Tile* tile = o2_world_getTileAtPos(&world, mouseWorldPos.x, mouseWorldPos.y);
					if (tile) {
						tile->id = WATER;
					}
				}
			}

			if (o2_time_limiterReady(&movementLimiter, currentTime)) {
				double timeDelta = currentTime - movementLimiter.last;
				movementLimiter.last = currentTime;
			 
				if (IsKeyDown(KEY_RIGHT)) {
					worldCamera.target.x += worldCameraSpeed * timeDelta;
				}
				if (IsKeyDown(KEY_LEFT)) {
					worldCamera.target.x -= worldCameraSpeed * timeDelta;
				}
				if (IsKeyDown(KEY_UP)) {
					worldCamera.target.y -= worldCameraSpeed * timeDelta;
				}
				if (IsKeyDown(KEY_DOWN)) {
					worldCamera.target.y += worldCameraSpeed * timeDelta;
				}
			}

			int fps = GetFPS();
			simpleNumToString(fpsDigits, fps, 7);

			if (o2_time_limiterResetIfReady(&fpsLimiter, currentTime)) {

				Vector2 worldCameraTopLeft = GetScreenToWorld2D((Vector2) { 0, 0 }, worldCamera);
				Vector2 worldCameraBottomRight = GetScreenToWorld2D((Vector2) { screenWidth, screenHeight }, worldCamera);

				// drawing
				BeginDrawing();
					// Setup the back buffer for drawing (clear color and depth buffers)
					ClearBackground(BLACK);
						
					BeginMode2D(worldCamera);
						o2_display_drawWorld(&world, worldCameraTopLeft, worldCameraBottomRight, tileSpriteSheet);
					EndMode2D();

					o2_display_drawBasePanel(&basePanel);

					// draw some text using the default font
					DrawText("Hello Raylib", 200, 200, 20, WHITE);
					DrawText(fpsDigits, 10, 10, 20, WHITE);

					// draw our texture to the screen
					DrawTexture(wabbit, 400, 200, WHITE);

				// end the frame and get ready for the next one  (display frame, poll input, etc...)
				EndDrawing();
			}
		}
	}

CLEAN_RESOURCES:
	if (IsTextureValid(wabbit)) {
		UnloadTexture(wabbit);
	}
	if (IsTextureValid(tilesTexture)) {
		UnloadTexture(tilesTexture);
	}
	if (IsWindowReady()) {
		CloseWindow();
	}

	return returnCode;
}

void printError(const char* message, const char* file, int lineNumber) {
	fprintf(stderr, "%s: (%s, %d)\n", message, file, lineNumber);
}

void simpleNumToString(char* output, int num, int maxDigits) {
	int digitCount = 0;

	while (num > 0 && digitCount < maxDigits) {
		output[digitCount] = (num % 10) + '0';
		num /= 10;
		digitCount++;
	}

	if (digitCount == 0) {
		output[0] = '0';
		digitCount++;
	}

	output[digitCount] = '\0';

	// reverse digits since they were created from least to most significant
	for (int i = 0; i < digitCount / 2; i++) {
		char temp = output[i];
		output[i] = output[digitCount - 1 - i];
		output[digitCount - 1 - i] = temp;
	}
}