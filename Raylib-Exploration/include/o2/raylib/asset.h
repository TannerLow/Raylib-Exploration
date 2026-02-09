#pragma once
#include <raylib.h>
#include <o2/arena.h>

typedef struct SpriteSheet {
	Texture texture;
	Rectangle* texCoords;
} SpriteSheet;

SpriteSheet o2_asset_createUniformSpriteSheet(o2_Arena* arena, Texture texture, int spriteWidth, int spriteHeight);