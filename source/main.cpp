#include <3ds.h>
#include <citro2d.h>
#include "menu.h"
#include "game.h"
#include "multiplayer.h"


int main(int argc, char **argv)
{
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    C3D_RenderTarget* top    = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    C3D_RenderTarget* bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    initMenu();
    initGame();
    initMultiplayer();

    enum AppState { STATE_MENU, STATE_LOADING, STATE_GAME, STATE_MULTIPLAYER };
    AppState currentState = STATE_MENU;
    
    bool isLoadingForMultiplayer = false;
    int loadingFrames = 0;
    const int totalLoadingFrames = 45;

    while (aptMainLoop())
    {
        hidScanInput();
        u32 kDown = hidKeysDown();
        touchPosition touch;
        hidTouchRead(&touch);

        //if (kDown & KEY_START) break;

        if (currentState == STATE_MENU)
        {
            int choice = updateMenu(kDown, touch);

            if (choice == 0) {           // PLAY vs AI
                isLoadingForMultiplayer = false;
                loadingFrames = 0;
                currentState = STATE_LOADING;
            } 
            else if (choice == 1) {      // MULTIPLAYER
                isLoadingForMultiplayer = true;
                loadingFrames = 0;
                currentState = STATE_LOADING;
            } 
            else if (choice == 2) {      // EXIT
                break;
            }
        }
        else if (currentState == STATE_LOADING)
        {
            loadingFrames++;
            if (loadingFrames >= totalLoadingFrames) {
                if (isLoadingForMultiplayer) {
                    resetMultiplayerBoard();
                    currentState = STATE_MULTIPLAYER;
                } else {
                    resetGameBoard();
                    currentState = STATE_GAME;
                }
                isLoadingForMultiplayer = false;
            }
        }
        else if (currentState == STATE_GAME)
        {
            updateGame(kDown, touch);

            if (kDown & KEY_SELECT) {
                resetGameBoard();        // Restart current game
            }
            else if (kDown & KEY_START) {
                currentState = STATE_MENU;   // Exit to main menu
            }
        }
        else if (currentState == STATE_MULTIPLAYER)
        {
            updateMultiplayer(kDown, touch);

            if (kDown & KEY_SELECT) {
                resetMultiplayerBoard();   // Restart current multiplayer game
            }
            else if (kDown & KEY_START) {
                currentState = STATE_MENU;   // Exit to main menu
            }
        }

        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

        if (currentState == STATE_MENU)
        {
            drawMenuTop(top);
            drawMenuBottom(bottom);
        }
        else if (currentState == STATE_LOADING)
        {
            C2D_SceneBegin(top);
            C2D_TargetClear(top, C2D_Color32(124, 118, 106, 255));
            C2D_DrawText(&textTitle, C2D_AlignCenter, 200, 95, 0.0f, 1.6f, 1.6f);

            C2D_SceneBegin(bottom);
            C2D_TargetClear(bottom, C2D_Color32(10, 10, 40, 255));

            float progress = (float)loadingFrames / totalLoadingFrames;
            C2D_DrawRectSolid(40, 105, 0.0f, 240, 18, C2D_Color32(50, 50, 70, 255));
            C2D_DrawRectSolid(40, 105, 0.0f, 240 * progress, 18, C2D_Color32(255, 230, 80, 255));
        }
        else if (currentState == STATE_GAME)
        {
            drawGameTop(top);
            drawGameBottom(bottom);
        }
        else if (currentState == STATE_MULTIPLAYER)
        {
            drawMultiplayerTop(top);
            drawMultiplayerBottom(bottom);
        }

        C3D_FrameEnd(0);
    }

    C2D_Fini();
    C3D_Fini();
    gfxExit();
    return 0;
}