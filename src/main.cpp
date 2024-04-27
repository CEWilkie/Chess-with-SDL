//
// Created by cew05 on 19/04/2024.
//

#define SDL_MAIN_HANDLED
#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>

#include "src_headers/GlobalSource.h"
#include "src_headers/Board.h"
#include "src_headers/Piece.h"

int EnsureWindowSize() {
    // Fetch rect of current window properties
    SDL_Rect rect_current = {0, 0};
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

    SDL_ClearError(); // clean up any other errors which may appear before deliberate error checking

    /*
     *  CONSTRUCT GAME ELEMENTS
     */

    // Construct Window Background
    window.background = IMG_LoadTexture(window.renderer, "../Resources/GameBoard/Green_Background.png");

    // Construct Board
    Board board;
    board.CreateBoardTexture();

    // Construct WHITE Pieces
    std::vector<Piece*> white_pieces;

    Piece pawn("Pawn", "White_Temp", {'A', 2});
    pawn.CreateTextures();
    pawn.GetRectOfBoardPosition(board);

    white_pieces.push_back(&pawn);

    // Construct BLACK Pieces
    std::vector<Piece*> black_pieces;

    bool running = true;
    while (running) {
        // Clear screen
        SDL_RenderClear(window.renderer);

        /*
         *  DRAW TO SCREEN
         */

        // Board and Background
        SDL_RenderCopy(window.renderer, window.background, nullptr, &window.currentRect);
        board.DisplayGameBoard();

        // White Pieces
        for (Piece* white_piece : white_pieces) {
            white_piece->DisplayPiece();
            white_piece->DisplayMoves(board);
        }

        /*
         *  EVENT MANAGEMENT
         */

        // User input events
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                mouseInput.active = true;
            }
            if (event.type == SDL_MOUSEBUTTONUP) {
                mouseInput.active = false;
            }
        }

        // piece click management
        for (Piece* piece : white_pieces) {
            // Check if piece is clicked on
            if (piece->IsClicked()){
                piece->FetchMoves(black_pieces, board);
            }


        }

        /*
         *  UPDATE SCREEN
         */

        SDL_RenderPresent(window.renderer);

        /*
         *  RECREATE TEXTURES IF REQUIRED
         */

        if (EnsureWindowSize() != 0){
            board.CreateBoardTexture();
            pawn.GetRectOfBoardPosition(board);
        }

    }

    SDL_Quit();
    TTF_Quit();

    return 1;
}