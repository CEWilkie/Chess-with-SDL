//
// Created by cew05 on 19/04/2024.
//

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>

#include <windows.h>
#include "StockfishUtil/StockfishManager.h"

#include "src_headers/GlobalSource.h"
#include "src_headers/GlobalResources.h"
#include "Screen/include/HomeScreen.h"
#include "Screen/include/GameScreen.h"

int EnsureWindowSize() {
    // Fetch rect of current window properties
    SDL_Rect rect_current = {0, 0, 0, 0};
    SDL_GetWindowSize(window.window, &rect_current.w, &rect_current.h);
    //SDL_GetWindowPosition(Window.window, &rect_current.a, &rect_current.b);

    // fetch min size boundaries
    int minW = 1020, minH = 540;

    // detect if window has changed size
    int changed = 0;
    if(rect_current.h != window.currentRect.h || rect_current.w != window.currentRect.w) { // changed (generic)
        changed = 1;
    }
    if (rect_current.w < minW) { // below min
        changed = -1;
        rect_current.w = minW;
    }
    if (rect_current.h < minH) { // below min
        changed = -1;
        rect_current.h = minH;
    }

    SDL_SetWindowSize(window.window, rect_current.w, rect_current.h);
    window.currentRect = rect_current;
    return changed;
}

int main(int argc, char** argv) {
    // Init SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        LogError("Issue initialising SDL", SDL_GetError(), true);
        return 0;
    }

    // Init TTF
    if (TTF_Init() != 0) {
        LogError("TTF failed to init", SDL_GetError(), true);
        return 0;
    }

    // produce window
    window.currentRect = {0, 0, 500, 500};
    window.minRect = window.currentRect;
    window.window = SDL_CreateWindow("ChesSDL", 0, 0, 500, 500, SDL_WINDOW_RESIZABLE);
    if (!window.window) {
        LogError("Failed to create window", SDL_GetError(), true);
        return 0;
    }

    // Create renderer
    window.renderer = SDL_CreateRenderer(window.window, 0, SDL_RENDERER_ACCELERATED);
    if (!window.renderer) {
        LogError("Failed to create renderer", SDL_GetError(), true);
        return 0;
    }

    // fetch border sizes and apply to position
    int b_top, b_left, b_right, b_bottom;
    if (SDL_GetWindowBordersSize(window.window, &b_top, &b_left, &b_bottom, &b_right) != 0) {
        LogError("Failed to retrive display borders size", SDL_GetError(), false);
    }

    // Set Window Background
    window.background = IMG_LoadTexture(window.renderer, "../Resources/Wood/Green_Background.png");

//    // fetch device screen dimensions and apply border sizes to w, h
//    if(SDL_GetDisplayUsableBounds(0, &window.currentRect) != 0) {
//        LogError("Failure to obtain display usable bounds", SDL_GetError(), false);
//    }
//
//    // Apply borders to position and dimensions of window
//    window.currentRect.w -= b_right + b_left;
//    window.currentRect.h -= b_top;

    SDL_SetWindowPosition(window.window, b_left, b_top);
    SDL_SetWindowSize(window.window, window.currentRect.w, window.currentRect.h);

    // clean up any other errors which may appear before deliberate error checking
    SDL_ClearError();

    /*
     * CREATE TEXTURES AND CONFIG FILES
     */

    // Initialise textures
    InitFonts();
    InitTextures();
    SetStyles();

    // Fetch and set config settings
    if (!ConfigExists()) {
        return -1;
    }

    /*
     *  CONSTRUCT SCREENS
     */

    SelectedPiece selectedPiece {};

    std::vector<AppScreen*> screens {};

    HomeScreen ms;
    ms.CreateTextures();

    GameScreen gs('W');
    gs.CreateTextures();
    gs.SetupEngine(true, 1320, 10);


    /*
     * LOAD STOCKFISH
     */

    /*
     * GAMELOOP
     */

    bool running = true;
    while (running) {
        // Clear screen
        SDL_RenderClear(window.renderer);

        /*
         *  DRAW TO SCREEN
         */

        //if (!ms.Display()) running = false;
        if (!gs.Display()) running = false;

        /*
         *  USER INPUT AND HANDLE EVENTS
         */

        ms.HandleEvents();
        ms.UpdateButtonStates();
        ms.CheckButtons();

        gs.HandleEvents();

        /*
         *  RECREATE TEXTURES IF REQUIRED
         */

        int winSizeChanged = EnsureWindowSize();
        if (winSizeChanged == 1) {

        }
        if (winSizeChanged != 0){
            ms.ResizeScreen();
            ms.CreateTextures();

            gs.ResizeScreen();
            gs.CreateTextures();
        }


        /*
         *  UPDATE SCREEN
         */

        SDL_RenderPresent(window.renderer);

        /*
         * CHECK FOR EXIT CONDITION
         */

        if (ms.FetchScreenState(AppScreen::ScreenState::SCREEN_CLOSED)) running = false;
        if (gs.FetchScreenState(GameScreen::GameState::CHECKMATE) ||
                gs.FetchScreenState(GameScreen::GameState::STALEMATE)) running = false;

    }

    SDL_Quit();
    TTF_Quit();

    return 1;
}