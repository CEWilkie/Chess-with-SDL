//
// Created by cew05 on 19/04/2024.
//

#define SDL_MAIN_HANDLED
#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>

#include "GlobalSource.h"
#include "Board.h"
#include "Piece.h"

int EnsureWindowSize() {
    // Fetch rect of current window properties
    SDL_Rect rect_current;
    SDL_GetWindowSize(window.window, &rect_current.w, &rect_current.h);
    //SDL_GetWindowPosition(Window.window, &rect_current.x, &rect_current.y);

    // detect if window has changed size
    int changed = 0;
    if(rect_current.h != window.currentRect.h || rect_current.w != window.currentRect.w) {
        changed = 1;
        window.currentRect = rect_current;
    }

    // enforce minimum size
    if (rect_current.w < 500) {
        SDL_SetWindowSize(window.window, 500, rect_current.h);
        changed = -1;
    }
    if (rect_current.h < 500) {
        SDL_SetWindowSize(window.window, rect_current.w, 500);
        changed = -1;
    }

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
        LogError("TTF failed to init", SDL_GetError(), false);
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

//    // fetch device screen dimensions and apply border sizes to w, h
//    if(SDL_GetDisplayUsableBounds(0, &window.currentRect) != 0) {
//        LogError("Failure to obtain display usable bounds", SDL_GetError(), false);
//    }
//
//    // Apply borders to position and dimensions of window
    window.currentRect.x = b_left;
    window.currentRect.y = b_top;
//    window.currentRect.w -= b_right + b_left;
//    window.currentRect.h -= b_top;

    SDL_SetWindowPosition(window.window, window.currentRect.x, window.currentRect.y);
    SDL_SetWindowSize(window.window, window.currentRect.w, window.currentRect.h);

    SDL_ClearError();

    // Construct Board
    Board board;

    // Construct Pieces
    Piece pawn("../Resources/Piece/Pawn_White_Temp.png", 250, 250);

    bool running = true;
    while (running) {
        // Clear screen
        SDL_RenderClear(window.renderer);

        // Draw to screen
        board.DisplayGameBoard();

        // Event management
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Update screen
        SDL_RenderPresent(window.renderer);



        if (EnsureWindowSize() != 0){
            board.CreateBoardTexture();
        }
    }



    return 1;
}