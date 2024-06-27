//
// Created by cew05 on 21/06/2024.
//

#ifndef CHESS_WITH_SDL_GAMESCREEN_H
#define CHESS_WITH_SDL_GAMESCREEN_H

#include <random>
#include "AppScreen.h"
#include "../../Gameplay/include/Board.h"
#include "../../Gameplay/include/IncludePieces.h"

class GameScreen : public AppScreen {
    private:
        enum GameState : int {
            SHOW_PROMO_MENU = LAST_SCREEN_STATE, END_OF_TURN, ALL_TASKS_COMPLETE,
        };
        enum MenuID : int {

        };
        enum buttonID : int {

        };

        // Pointers to board and pieces
        Board* board;
        SelectedPiece* selectedPiece = new SelectedPiece();
        std::vector<Piece*>* allPieces = new std::vector<Piece*>;
        std::vector<Piece*>* whitePieces = new std::vector<Piece*>;
        std::vector<Piece*>* blackPieces = new std::vector<Piece*>;
        std::vector<Piece*>* teamptr = nullptr;
        std::vector<Piece*>* oppptr = nullptr;

        // Stockfish


    public:
        GameScreen();

        // Game setup
        void SetUpPieces();

        // Display
        bool CreateTextures() override;
        bool Display() override;
        void ResizeScreen() override;

        // Handle events
        void HandleEvents() override;
        void CheckButtons() override;
        void GetOpponentAIMove();
        void GetOpponentNetworkMove();
};


#endif //CHESS_WITH_SDL_GAMESCREEN_H
