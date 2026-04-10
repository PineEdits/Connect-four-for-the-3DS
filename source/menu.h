#pragma once
#include <3ds.h>
#include <citro2d.h>

extern C2D_Text textTitle;
extern C2D_Text textControls;
extern C2D_Text textRestart;
extern C2D_Text textPlay;

extern bool wasTouching;

void initMenu();
void drawMenuTop(C3D_RenderTarget* top);
void drawMenuBottom(C3D_RenderTarget* bottom);
int  updateMenu(u32 kDown, touchPosition touch);