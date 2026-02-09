#include <o2/raylib/asset.h>
#include <assert.h>

SpriteSheet o2_asset_createUniformSpriteSheet(o2_Arena* arena, Texture texture, int spriteWidth, int spriteHeight) {
	assert(NULL != arena);
	assert(IsTextureValid(texture));
	assert(0 != spriteWidth);
	assert(0 != spriteHeight);

	SpriteSheet sheet;

	int spriteCount = (texture.width / spriteWidth) * (texture.height / spriteHeight);
	Rectangle* texCoords = (Rectangle*)o2_arena_pushArray(arena, sizeof(Rectangle), spriteCount);
	int rows = (texture.height / spriteHeight);
	int cols = (texture.width / spriteWidth);

	for (int row = 0; row < rows; row++) {
		for (int col = 0; col < cols; col++) {
			Rectangle* rect = texCoords + (row * cols + col);
			rect->x = col * spriteWidth;
			rect->y = row * spriteHeight;
			rect->width = spriteWidth;
			rect->height = spriteHeight;
		}
	}

	sheet.texture = texture;
	sheet.texCoords = texCoords;

	return sheet;
}