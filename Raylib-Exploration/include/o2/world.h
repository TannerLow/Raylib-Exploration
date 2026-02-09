#pragma once
#include <o2/types.h>
#include <o2/arena.h>

typedef enum o2_TileID {
	VOID,
	GRASS,
	DIRT,
	WATER
} o2_TileID;

typedef struct o2_Tile {
	o2_TileID id;
	uint8 row;
	uint8 col;
} o2_Tile;

typedef struct o2_Chunk {
	o2_Tile* tiles;
	uint8 rows;
	uint8 cols;
	int worldX;
	int worldY;
} o2_Chunk;

typedef struct o2_World {
	o2_Chunk* chunks;
	int rows;
	int cols;
	int tileWidth;
	int tileHeight;
} o2_World;

typedef struct o2_WorldSettings {
	int rowsOfChunks;
	int colsOfChunks;
	uint8 rowsOfTiles;
	uint8 colsOfTiles;
	int tileWidth;
	int tileHeight;
} o2_WorldSettings;

o2_World o2_world_createWorld(o2_Arena* arena, o2_WorldSettings settings);
o2_Tile* o2_world_getTileAtPos(o2_World* world, float x, float y);