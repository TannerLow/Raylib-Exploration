#pragma once
#include <o2/types.h>

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
