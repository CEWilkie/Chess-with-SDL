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
#include "src_headers/SelectedPiece.h"

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

    // clean up any other errors which may appear before deliberate error checking
    SDL_ClearError();

    /*
     *  CONSTRUCT GAME ELEMENTS
     */

    // Construct Window Background
    window.background = IMG_LoadTexture(window.renderer, "../Resources/GameBoard/Green_Background.png");

    // Construct Board
    Board board;
    board.CreateBoardTexture();

    /*
     * CONSTRUCT WHITE PIECES
     */
    std::vector<Piece*> all_pieces;
    std::vector<Piece*> white_pieces;

    // Create Pawns
    for (int p = 0; p < 8; p++) {
        // Create new pawn
        auto* pawn = new Piece("Pawn", "White_Temp", {char('A'+p), 2});
        pawn->CreateTextures();
        pawn->GetRectOfBoardPosition(board);

        // add to pieces vector
        white_pieces.push_back(pawn);
        all_pieces.push_back(pawn);
    }

    auto* test_pawn = new Piece("Pawn", "White_Temp", {'A', 3});
    test_pawn->CreateTextures();
    test_pawn->GetRectOfBoardPosition(board);
    white_pieces.push_back(test_pawn);
    all_pieces.push_back(test_pawn);

    /*
     * CONSTRUCT BLACK PIECES
     */
     std::vector<Piece*> black_pieces;

    // Create Pawns
    for (int p = 0; p < 8; p++) {
        // Create new pawn
        auto* pawn = new Piece("Pawn", "Black_Temp", {char('A'+p), 7});
        pawn->CreateTextures();
        pawn->GetRectOfBoardPosition(board);

        // add to pieces vector
        black_pieces.push_back(pawn);
        all_pieces.push_back(pawn);
    }

    test_pawn = new Piece("Pawn", "Black_Temp", {'B', 3});
    test_pawn->CreateTextures();
    test_pawn->GetRectOfBoardPosition(board);
    black_pieces.push_back(test_pawn);
    all_pieces.push_back(test_pawn);

    /*
     * CONSTRUCT ADDITIONAL
     */

    auto* teamptr = &white_pieces;
    auto* oppptr = &black_pieces;
    SelectedPiece selectedPiece(teamptr, oppptr, &board);

    bool running = true;
    while (running) {
        // Clear screen
        SDL_RenderClear(window.renderer);

        /*
         *  DRAW TO SCREEN
         */

        // Display Board and Background
        SDL_RenderCopy(window.renderer, window.background, nullptr, &window.currentRect);
        board.DisplayGameBoard();

        // Display Pieces
        for (Piece* piece : all_pieces) {
            piece->DisplayPiece();
            piece->DisplayMoves(board);
        }

        /*
         *  FETCH USER INPUT FROM EVENTS
         */

        mouseInput.prevactive = mouseInput.active;

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

        // Update mouse
        mouseInput.UpdateState();

        /*
         * EVENT MANAGEMENT
         */

        // First check if user has clicked on an available move space / dragged piece to available move space
        if (mouseInput.active) {
            if (selectedPiece.MadeMove(oppptr)) {
                printf("move made!");
                selectedPiece.SwapPieceSetPointers();
                std::swap(teamptr, oppptr);
            }
        }

        // On user click
        if (mouseInput.active && ! mouseInput.heldactive) {
            // User did not click on available move, so check if a different piece was clicked
            if (!std::any_of(teamptr->begin(), teamptr->end(), [&selectedPiece](Piece* piece)
            {
                if (piece->CheckClicked()) {
                    // piece was clicked, so update the selected piece
                    selectedPiece.ChangeSelectedPiece(piece);
                    return true;
                }
                return false;
            })) {
                // no piece was selected on click
                selectedPiece.ChangeSelectedPiece(nullptr);
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
            // board
            board.CreateBoardTexture();

            // white pieces
            for (Piece* piece : white_pieces) {
                piece->GetRectOfBoardPosition(board);
            }

            // black pieces
            for (Piece* piece : black_pieces) {
                piece->GetRectOfBoardPosition(board);
            }
        }

    }

    SDL_Quit();
    TTF_Quit();

    return 1;
}