#pragma once
#include <raylib.h>
#include <o2/world.h>
#include <o2/raylib/asset.h>

typedef struct Tab {
	Rectangle bounds;
	Color color;
} Tab;

typedef struct BasePanel {
	Rectangle bounds;
	Color bgColor;
	Rectangle iconPanelBounds;
	Color iconPanelColor;
	Tab sideTabs[4];
	Tab topTabs[5];
} BasePanel;

void o2_display_drawBasePanel(BasePanel* panel);
void o2_display_initBasePanel(BasePanel* panel, Rectangle bounds);

void o2_display_drawWorld(
	o2_World* world, 
	Vector2 cameraTopLeft, 
	Vector2 cameraBottomRight, 
	SpriteSheet tileSpriteSheet
);