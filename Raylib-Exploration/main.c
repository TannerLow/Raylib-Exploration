#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <o2/time.h>
#include <string.h>
#include <o2/arena.h>
#include <o2/world.h>
#include <assert.h>

void printError(const char* message, const char* file, int lineNumber);
void simpleNumToString(char* output, int num, int maxDigits);

int main() {
	int returnCode = EXIT_SUCCESS;
	
	Texture wabbit = { 0 };
	Texture tileSpriteSheet = { 0 };

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

		tileSpriteSheet = LoadTexture("resources/tile_sheet.png");
		if (!IsTextureValid(tileSpriteSheet)) {
			printError("Failed to load texture: resources/tile_sheet.png", __FILE__, __LINE__);
			returnCode = EXIT_FAILURE;
			goto CLEAN_RESOURCES;
		}
		int tileWidth = 32;
		int tileHeight = 32;
		int tileSpriteCount = (tileSpriteSheet.width / tileWidth) * (tileSpriteSheet.height / tileHeight);
		Rectangle* tileTexCoords = (Rectangle*)o2_arena_pushArray(&globalArena, sizeof(Rectangle), tileSpriteCount);
		{
			int rows = (tileSpriteSheet.height / tileHeight);
			int cols = (tileSpriteSheet.width / tileWidth);
			for (int row = 0; row < rows; row++) {
				for (int col = 0; col < cols; col++) {
					Rectangle* rect = tileTexCoords + (row * cols + col);
					rect->x = col * tileWidth;
					rect->y = row * tileHeight;
					rect->width = tileWidth;
					rect->height = tileHeight;
				}
			}
		}
		assert(tileSpriteCount == 4);

		Camera2D worldCamera = {
			.offset = (Vector2){width / 2.0, height / 2.0},
			.target = (Vector2){0, 0},
			.rotation = 0,
			.zoom = 1.f
		};
		int worldCameraSpeed = 200;

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
		o2_Chunk* chunks = (o2_Chunk*)o2_arena_pushArray(&globalArena, sizeof(o2_Chunk), rowsOfChunks * colsOfChunks);
		
		for (int chunkRow = 0; chunkRow < rowsOfChunks; chunkRow++) {
			for (int chunkCol = 0; chunkCol < colsOfChunks; chunkCol++) {
				
				o2_Chunk* chunk = chunks + (chunkRow * colsOfChunks + chunkCol);
				chunk->rows = chunkRows;
				chunk->cols = chunkCols;
				chunk->worldX = chunkCol + minChunkX;
				chunk->worldY = chunkRow + minChunkY;
				chunk->tiles = (o2_Tile*)o2_arena_pushArray(&globalArena, sizeof(o2_Tile), chunkRows * chunkCols);

				for (uint8 row = 0; row < chunkRows; row++) {
					for (uint8 col = 0; col < chunkCols; col++) {
						
						o2_Tile* tile = chunk->tiles + (row * chunk->cols + col);
						tile->row = row;
						tile->col = col;
						tile->id = GRASS;

					}
				}
			}
		}

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
				int minVisibleChunkX = worldCameraTopLeft.x / chunkWidth;
				int minVisibleChunkY = worldCameraTopLeft.y / chunkHeight;
				int maxVisibleChunkX = worldCameraBottomRight.x / chunkWidth;
				int maxVisibleChunkY = worldCameraBottomRight.y / chunkHeight;
				if (worldCameraTopLeft.x < 0) minVisibleChunkX--;
				if (worldCameraTopLeft.y < 0) minVisibleChunkY--;
				if (worldCameraBottomRight.x < 0) maxVisibleChunkX--;
				if (worldCameraBottomRight.y < 0) maxVisibleChunkY--;

				// drawing
				BeginDrawing();
					// Setup the back buffer for drawing (clear color and depth buffers)
					ClearBackground(BLACK);
						
					BeginMode2D(worldCamera);
					for (int row = minVisibleChunkY; row <= maxVisibleChunkY; row++) {
						if (minChunkY <= row and row < maxChunkY) {
							for (int col = minVisibleChunkX; col <= maxVisibleChunkX; col++) {
								if (minChunkX <= col and col < maxChunkX) {
									
									o2_Chunk* chunk = chunks + ((row - minChunkY) * colsOfChunks + (col - minChunkX));
									int chunkX = chunk->worldX * chunkWidth;
									int chunkY = chunk->worldY * chunkHeight;
									
									for (uint8 tileRow = 0; tileRow < chunk->rows; tileRow++) {
										for (uint8 tileCol = 0; tileCol < chunk->cols; tileCol++) {

											Vector2 position = (Vector2) {
												tileCol * tileWidth + chunkX, 
												tileRow * tileHeight + chunkY
											};
											o2_Tile* tile = chunk->tiles + (tileRow * chunk->cols + tileRow);
											DrawTextureRec(tileSpriteSheet, tileTexCoords[tile->id], position, WHITE);
										}
									}
								}
							}
						}
					}
					EndMode2D(worldCamera);

					//DrawRectangle(0, 0, 1280, 720, WHITE);

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
	if (IsTextureValid(tileSpriteSheet)) {
		UnloadTexture(tileSpriteSheet);
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