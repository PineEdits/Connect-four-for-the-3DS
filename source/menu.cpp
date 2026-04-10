#include "menu.h"
#include "game.h"
#include <stdio.h>

C2D_TextBuf menuTextBuf;
C2D_Text textTitle, textPlay, textMulti, textExit, textControls, textAuther;

int menuSelection = 0;
bool wasTouching = false;

void initMenu() {
    menuTextBuf = C2D_TextBufNew(4096);

    C2D_TextParse(&textTitle,    menuTextBuf, "CONNECT FOUR");
    C2D_TextParse(&textPlay,     menuTextBuf, "SINGLEPLAYER");
    C2D_TextParse(&textMulti,    menuTextBuf, "MULTIPLAYER");
    C2D_TextParse(&textExit,     menuTextBuf, "EXIT");
    C2D_TextParse(&textControls, menuTextBuf, "↑↓ + A or Touch and release");
    C2D_TextParse(&textAuther,   menuTextBuf, "BY CHARLES TARLETON");

}

void drawMenuTop(C3D_RenderTarget* top) {

    C2D_SceneBegin(top);
    //C2D_TargetClear(top, C2D_Color32(5, 5, 35, 255));
    C2D_TargetClear(top, C2D_Color32(124, 118, 106, 255)); 

    C2D_DrawText(&textTitle, C2D_AlignCenter, 200, 45, 0.0f, 1.9f, 1.9f, COLOR_WHITE);

    C2D_DrawRectSolid(80, 85, 0.0f, 240, 6, C2D_Color32(255, 215, 0, 255));
    C2D_DrawRectSolid(80, 95, 0.0f, 240, 6, C2D_Color32(255, 215, 0, 255));

    C2D_DrawCircleSolid(100, 140, 0.0f, 28, COLOR_RED);
    C2D_DrawCircleSolid(300, 140, 0.0f, 28, COLOR_YELLOW);

    C2D_DrawText(&textAuther, 0, 15, 220, 0.0f, 0.65f, 0.65f);
}

void drawMenuBottom(C3D_RenderTarget* bottom) {
    C2D_SceneBegin(bottom);
    C2D_TargetClear(bottom, C2D_Color32(25, 25, 70, 255));

    // PLAY vs AI
    if (menuSelection == 0) {
        C2D_DrawRectSolid(48, 42, 0.0f, 224, 52, C2D_Color32(255, 240, 100, 130));
        C2D_DrawTriangle(25, 59, 25, 79, 8, 69,
                         C2D_Color32(255,255,100,255),
                         C2D_Color32(255,255,100,255),
                         C2D_Color32(255,255,100,255), 0.0f);
    }
    C2D_DrawText(&textPlay, C2D_AlignCenter, 160, 51, 0.0f,
                 menuSelection==0?1.12f:1.0f,
                 menuSelection==0?1.12f:1.0f);

    // MULTIPLAYER
    if (menuSelection == 1) {
        C2D_DrawRectSolid(48, 102, 0.0f, 224, 52, C2D_Color32(255, 240, 100, 130));
        C2D_DrawTriangle(25, 119, 25, 139, 8, 129,
                         C2D_Color32(255,255,100,255),
                         C2D_Color32(255,255,100,255),
                         C2D_Color32(255,255,100,255), 0.0f);
    }
    C2D_DrawText(&textMulti, C2D_AlignCenter, 160, 111, 0.0f,
                 menuSelection==1?1.12f:1.0f,
                 menuSelection==1?1.12f:1.0f);

    // EXIT
    if (menuSelection == 2) {
        C2D_DrawRectSolid(48, 162, 0.0f, 224, 52, C2D_Color32(255, 240, 100, 130));
        C2D_DrawTriangle(25, 179, 25, 199, 8, 189,
                         C2D_Color32(255,255,100,255),
                         C2D_Color32(255,255,100,255),
                         C2D_Color32(255,255,100,255), 0.0f);
    }
    C2D_DrawText(&textExit, C2D_AlignCenter, 160, 171, 0.0f,
                 menuSelection==2?1.12f:1.0f,
                 menuSelection==2?1.12f:1.0f);

    C2D_DrawText(&textControls, 0, 15, 220, 0.0f, 0.65f, 0.65f);
}

int updateMenu(u32 kDown, touchPosition touch) {
    // D-Pad
    if (kDown & (KEY_UP | KEY_DUP))    menuSelection = 0;
    if (kDown & (KEY_DOWN | KEY_DDOWN)) menuSelection = (menuSelection + 1) % 3;

    // Touch: hover while holding, confirm on release
    bool isTouching = (touch.px != 0 || touch.py != 0);

    if (isTouching) {
        if (touch.py < 90)      menuSelection = 0;
        else if (touch.py < 150) menuSelection = 1;
        else                    menuSelection = 2;
    } 
    else if (wasTouching) {
        return menuSelection;
    }

    wasTouching = isTouching;

    if (kDown & KEY_A) {
        return menuSelection;
    }

    return -1;
}