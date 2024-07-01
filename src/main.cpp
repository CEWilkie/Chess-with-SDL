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
#include "UserInterface/include/HomeScreen.h"
#include "UserInterface/include/GameScreen.h"

int EnsureWindowSize() {
    // Fetch rect of current window properties
    SDL_Rect rect_current = {0, 0, 0, 0};
    SDL_GetWindowSize(window.window, &rect_current.w, &rect_current.h);
    //SDL_GetWindowPosition(Window.window, &rect_current.a, &rect_current.b);

    // fetch min size boundaries
    int minW = 900, minH = 540;

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

    // Home screen / Main Menu for the user
    HomeScreen hs;
    hs.CreateTextures();

    screenManager->NewResource(&hs, HOMESCREEN);

    // Provides view of the gameboard during gameplay vs ai / network / game review
    GameScreen gs('W');
    gs.CreateTextures();

    // Later should move this
    gs.SetupEngine(true, 1320, 10);

    screenManager->NewResource(&gs, GAMESCREEN);

    // Loop utilises pointer to the current screen. Prevents multiple screens from attempting to manage events which
    // can lead to conflicts. Appscreen vector holds pointers to all current screens.
    screenManager->FetchResource(currentScreen, HOMESCREEN);

    /*
     * GAMELOOP
     */

    // Set initial values of vars
    frameTick.currTick = SDL_GetTicks64();
    bool running = true;

    // Loop

    while (running) {
        // Update ticks
        frameTick.lastTick = frameTick.currTick;
        frameTick.currTick = SDL_GetTicks64();
        frameTick.tickChange = frameTick.currTick - frameTick.lastTick;

        // Clear screen
        SDL_RenderClear(window.renderer);

        /*
         *  DRAW TO SCREEN
         */

        if (!currentScreen->Display())
            running = false;

        /*
         *  USER INPUT AND HANDLE EVENTS
         */

        // Handle events called first as this updates MouseInput vars (such as mouse down) required for updating button
        // states
        currentScreen->HandleEvents();
        currentScreen->UpdateButtonStates();
        currentScreen->CheckButtons();

        /*
         *  RECREATE TEXTURES IF REQUIRED
         */

        int winSizeChanged = EnsureWindowSize();
        if (winSizeChanged == 1) {
            // ...
        }
        if (winSizeChanged != 0){
            currentScreen->ResizeScreen();
            currentScreen->CreateTextures();
        }

        /*
         *  UPDATE SCREEN
         */

        SDL_RenderPresent(window.renderer);

        /*
         * CHECK FOR EXIT CONDITION
         */

        if (currentScreen->FetchScreenState(AppScreen::ScreenState::WINDOW_CLOSED)) running = false;
    }

    SDL_Quit();
    TTF_Quit();

    return 1;
}