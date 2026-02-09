#include <o2/raylib/display.h>
#include <assert.h>

void o2_display_drawBasePanel(BasePanel* panel) {
	assert(NULL != panel);

	DrawRectangleRec(panel->bounds, panel->bgColor);
	DrawRectangleRec(panel->iconPanelBounds, panel->iconPanelColor);
	for (int i = 0; i < 4; i++) {
		DrawRectangleRec(panel->sideTabs[i].bounds, panel->sideTabs[i].color);
	}
	for (int i = 0; i < 5; i++) {
		DrawRectangleRec(panel->topTabs[i].bounds, panel->topTabs[i].color);
	}
}

void o2_display_initBasePanel(BasePanel* panel, Rectangle bounds) {
	assert(NULL != panel);

	Rectangle iconPanelRect = (Rectangle){ 112, bounds.y + 48, bounds.width - 128, bounds.height - 64 };

	panel->bounds = bounds;
	panel->bgColor = (Color){ 179, 79, 79, 255 };
	panel->iconPanelBounds = iconPanelRect;
	panel->iconPanelColor = (Color){ 131, 151, 151, 255 };

	for (int i = 0; i < 4; i++) {
		panel->sideTabs[i].bounds = (Rectangle){
			iconPanelRect.x - 80, 
			iconPanelRect.y + i * 40,
			80,
			32
		};
		panel->sideTabs[i].color = (Color){ 111, 131, 131, 255 };
	}

	for (int i = 0; i < 5; i++) {
		panel->topTabs[i].bounds = (Rectangle){
			16 + iconPanelRect.x + i * 144,
			iconPanelRect.y - 32,
			128,
			32
		};
		panel->topTabs[i].color = (Color){ 111, 131, 131, 255 };
	}
}

void o2_display_drawWorld(
	o2_World* world, 
	Vector2 cameraTopLeft, 
	Vector2 cameraBottomRight, 
	SpriteSheet tileSpriteSheet
) {
	assert(NULL != world);
	assert(world->cols > 0);
	assert(world->rows > 0);
	assert(NULL != tileSpriteSheet.texCoords);

	int minChunkX = 0 - world->cols / 2;
	int minChunkY = 0 - world->rows / 2;
	int maxChunkX = minChunkX + world->cols - 1;
	int maxChunkY = minChunkY + world->rows - 1;
	int chunkWidth  = world->tileWidth  * world->chunks[0].cols;
	int chunkHeight = world->tileHeight * world->chunks[0].rows;

	int minVisibleChunkX = cameraTopLeft.x / chunkWidth;
	int minVisibleChunkY = cameraTopLeft.y / chunkHeight;
	int maxVisibleChunkX = cameraBottomRight.x / chunkWidth;
	int maxVisibleChunkY = cameraBottomRight.y / chunkHeight;
	if (cameraTopLeft.x < 0) minVisibleChunkX--;
	if (cameraTopLeft.y < 0) minVisibleChunkY--;
	if (cameraBottomRight.x < 0) maxVisibleChunkX--;
	if (cameraBottomRight.y < 0) maxVisibleChunkY--;

	for (int row = minVisibleChunkY; row <= maxVisibleChunkY; row++) {
		if (minChunkY <= row and row <= maxChunkY) {
			for (int col = minVisibleChunkX; col <= maxVisibleChunkX; col++) {
				if (minChunkX <= col and col <= maxChunkX) {

					o2_Chunk* chunk = world->chunks + ((row - minChunkY) * world->cols + (col - minChunkX));
					int chunkX = chunk->worldX * chunkWidth;
					int chunkY = chunk->worldY * chunkHeight;

					for (uint8 tileRow = 0; tileRow < chunk->rows; tileRow++) {
						for (uint8 tileCol = 0; tileCol < chunk->cols; tileCol++) {

							Vector2 position = (Vector2){
								tileCol * world->tileWidth + chunkX,
								tileRow * world->tileHeight + chunkY
							};
							o2_Tile* tile = chunk->tiles + (tileRow * chunk->cols + tileCol);
							if (tile->id != GRASS) {
								int x = 9;
							}
							DrawTextureRec(tileSpriteSheet.texture, tileSpriteSheet.texCoords[tile->id], position, WHITE);
						}
					}
				}
			}
		}
	}
}
