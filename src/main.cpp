//
// Created by cew05 on 19/04/2024.
//

#define SDL_MAIN_HANDLED
#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <fstream>

#include "src_headers/GlobalSource.h"
#include "src_headers/GlobalResources.h"
#include "Gameplay/include/Board.h"
#include "Gameplay/include/Piece.h"
#include "Gameplay/include/SelectedPiece.h"
#include "Gameplay/include/Knight.h"
#include "Gameplay/include/Bishop.h"
#include "Gameplay/include/Rook.h"
#include "Gameplay/include/Queen.h"
#include "Gameplay/include/King.h"
#include "Screen/include/HomeScreen.h"
#include "Screen/include/GameScreen.h"

int EnsureWindowSize() {
    // Fetch rect of current window properties
    SDL_Rect rect_current = {0, 0, 0, 0};
    SDL_GetWindowSize(window.window, &rect_current.w, &rect_current.h);
    //SDL_GetWindowPosition(Window.window, &rect_current.a, &rect_current.b);

    // fetch min size boundaries
    int minW = 700, minH = 500;

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
     * CREATE TEXTURES
     */

    InitFonts();
    InitTextures();
    SetStyles();

    /*
     * CONFIG FILES
     */

    if (!ConfigExists()) {
        return -1;
    }

    /*
     *  CONSTRUCT GAME ELEMENTS
     */

    SelectedPiece selectedPiece {};

    std::vector<AppScreen*> screens {};

    HomeScreen ms;
    GameScreen gs;
    ms.CreateTextures();
    gs.CreateTextures();

    bool running = true;
    bool eot = false;
    bool allTasksComplete = true;
    while (running) {
        // Clear screen
        SDL_RenderClear(window.renderer);

        /*
         *  DRAW TO SCREEN
         */

//        // DisplayToggle Board and Background
//        SDL_RenderCopy(window.renderer, window.background, nullptr, &window.currentRect);
//        board.DisplayGameBoard();
//
//        // DisplayToggle Pieces and fetch their moves
//        for (Piece* piece : allPieces) {
//            piece->ClearMoves();
//            piece->ClearNextMoves();
//            piece->FetchMoves(*teamptr, *oppptr, board);
//            piece->PreventMoveIntoCheck(*teamptr, *oppptr, board);
//
//            piece->DisplayPiece();
//            piece->DisplayMoves(board);
//        }

        //if (!ms.Display()) running = false;
        if (!gs.Display()) running = false;


        /*
         * CHECKMATE + STALEMATE CHECKING
         * this is done after moves are fetched and confirmed
         */


        /*
         *  FETCH USER INPUT FROM EVENTS
         */

        // User input events
        ms.HandleEvents();
        ms.UpdateButtonStates();
        ms.CheckButtons();




        /*
         * EVENT MANAGEMENT - MOVES AND PIECE SELECTION CHECKING
         */

        // check if user has clicked on a move
//        if (selectedPiece.CheckForMoveClicked(&board)) {
//            // make move
//            selectedPiece.MakeMove(&board);
//            eot = true;
//        }
//
//        // check if user clicks on a piece
//        if (!eot) selectedPiece.CheckForPieceClicked(teamptr);
//
//        /*
//         *  END OF TURN MANAGEMENT
//         */
//
//        // Do promotion if pawn has reached the end tile
//        if (eot) {
//            for (auto piece : *teamptr) {
//                if (piece->ReadyToPromote()) {
//                    allTasksComplete = false;
//
//                    // Fetch user input for promotion
//                    board.DisplayPromoMenu(piece);
//
//                    Piece* promotedPiece = nullptr;
//                    std::string promoteTo = board.GetPromoMenuInput();
//                    Piece_Info* pi = piece->GetPieceInfoPtr();
//
//                    if (promoteTo == "Knight") {
//                        promotedPiece = new Knight("Knight", pi->colID, pi->gamepos);
//                    }
//                    else if (promoteTo == "Bishop") {
//                        promotedPiece = new Bishop("Bishop", pi->colID, pi->gamepos);
//                    }
//                    else if (promoteTo == "Rook") {
//                        promotedPiece = new Rook("Rook", pi->colID, pi->gamepos);
//                    }
//                    else if (promoteTo == "Queen"){
//                        promotedPiece = new Queen("Queen", pi->colID, pi->gamepos);
//                    }
//
//                    if (promotedPiece != nullptr) {
//                        // Piece has been made, mark pawn as captured and add new piece to teamptr
//                        piece->Captured(true);
//                        piece->UpdatePromoteInfo(promotedPiece);
//
//                        promotedPiece->CreateTextures();
//                        promotedPiece->GetRectOfBoardPosition(board);
//
//                        teamptr->push_back(promotedPiece);
//                        allPieces.push_back(promotedPiece);
//
//                        promotedPiece->FetchMoves(*teamptr, *oppptr, board);
//
//                        allTasksComplete = true;
//                    }
//                }
//            }
//        }
//
//        if (eot && allTasksComplete) {
//            // update checker vars
//            for (auto piece : allPieces) {
//                piece->UpdateCheckerVars();
//            }
//
//            // Create and get the lastMove string
//            selectedPiece.CreateACNstring(teamptr);
//            board.WriteMoveToFile(selectedPiece.GetACNMoveString());
//
//            std::swap(teamptr, oppptr);
//            board.IncrementTurn();
//
//            eot = false;
//        }

        /*
         *  RECREATE TEXTURES IF REQUIRED
         */

        int winSizeChanged = EnsureWindowSize();
        if (winSizeChanged == 1) {
            // Update board size
//            int w, h;
//            SDL_GetWindowSize(window.window, &w, &h);
//            board.FillToBounds(w, h);
        }
        if (winSizeChanged != 0){
            // board
//            board.CreateBoardTexture();
//            board.CreatePromoMenuTexture();
//
//            // white pieces
//            for (Piece* piece : white_pieces) {
//                piece->GetRectOfBoardPosition(board);
//            }
//
//            // black pieces
//            for (Piece* piece : black_pieces) {
//                piece->GetRectOfBoardPosition(board);
//            }

            ms.ResizeScreen();
            ms.CreateTextures();

            gs.ResizeScreen();
            gs.CreateTextures();
        }


        /*
         *  UPDATE SCREEN
         */

        // Example background Fill

        SDL_RenderPresent(window.renderer);

        /*
         * CHECK FOR EXIT CONDITION
         */

        if (ms.FetchScreenState(AppScreen::ScreenState::SCREEN_CLOSED)) running = false;
    }

    SDL_Quit();
    TTF_Quit();

    return 1;
}