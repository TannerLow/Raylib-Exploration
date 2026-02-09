#include <o2/world.h>
#include <assert.h>

o2_World o2_world_createWorld(o2_Arena* arena, o2_WorldSettings settings) {
	assert(NULL != arena);

	o2_World world;

	int chunkCount = settings.rowsOfChunks* settings.colsOfChunks;
	o2_Chunk* chunks = (o2_Chunk*)o2_arena_pushArray(arena, sizeof(o2_Chunk), chunkCount);

	int minChunkX = 0 - settings.colsOfChunks / 2;
	int minChunkY = 0 - settings.rowsOfChunks / 2;

	for (int chunkRow = 0; chunkRow < settings.rowsOfChunks; chunkRow++) {
		for (int chunkCol = 0; chunkCol < settings.colsOfChunks; chunkCol++) {

			o2_Chunk* chunk = chunks + (chunkRow * settings.colsOfChunks + chunkCol);
			chunk->rows = settings.rowsOfTiles;
			chunk->cols = settings.colsOfTiles;
			chunk->worldX = chunkCol + minChunkX;
			chunk->worldY = chunkRow + minChunkY;
			chunk->tiles = (o2_Tile*)o2_arena_pushArray(arena, sizeof(o2_Tile), chunk->rows * chunk->cols);

			for (uint8 row = 0; row < chunk->rows; row++) {
				for (uint8 col = 0; col < chunk->cols; col++) {

					o2_Tile* tile = chunk->tiles + (row * chunk->cols + col);
					tile->row = row;
					tile->col = col;
					tile->id = GRASS;

				}
			}
		}
	}

	world.chunks = chunks;
	world.cols = settings.colsOfChunks;
	world.rows = settings.rowsOfChunks;
	world.tileWidth = settings.tileWidth;
	world.tileHeight = settings.tileHeight;

	return world;
}

o2_Tile* o2_world_getTileAtPos(o2_World* world, float x, float y) {
	assert(NULL != world);
	assert(NULL != world->chunks);

	int minChunkX = 0 - world->cols / 2;
	int minChunkY = 0 - world->rows / 2;
	int maxChunkX = minChunkX + world->cols - 1;
	int maxChunkY = minChunkY + world->rows - 1;
	int chunkWidth = world->tileWidth * world->chunks[0].cols;
	int chunkHeight = world->tileHeight * world->chunks[0].rows;

	int chunkX = x / chunkWidth;
	int chunkY = y / chunkHeight;
	if (x < 0) chunkX--;
	if (y < 0) chunkY--;

	if (minChunkX <= chunkX and chunkX <= maxChunkX) {
		if (minChunkY <= chunkY and chunkY < maxChunkY) {
			int localX = x - chunkX * chunkWidth;
			int localY = y - chunkY * chunkHeight;
			int tileCol = localX / world->tileWidth;
			int tileRow = localY / world->tileHeight;
			o2_Chunk* chunk = world->chunks + ((chunkY - minChunkY) * world->cols + (chunkX - minChunkX));
			return chunk->tiles + (tileRow * chunk->cols + tileCol);
		}
	}

	return NULL;
}