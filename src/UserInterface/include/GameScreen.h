//
// Created by cew05 on 21/06/2024.
//

#ifndef CHESS_WITH_SDL_GAMESCREEN_H
#define CHESS_WITH_SDL_GAMESCREEN_H

#include <random>
#include "AppScreen.h"
#include "../../Gameplay/include/Board.h"
#include "../../Gameplay/include/IncludePieces.h"
#include "../../StockfishUtil/StockfishManager.h"

class GameScreen : public AppScreen {
    public:
        enum GameState : int {
            SHOW_PROMO_MENU = LAST_SCREEN_STATE, END_OF_TURN, ALL_TASKS_COMPLETE, CHECKMATE, STALEMATE,
            DRAW_OFFER, RESIGN, BOARD_FLIPPED,
        };

    private:
        enum MenuID : int {
            OPTIONS_MENU, GAME_OVER,
        };
        enum buttonID : int {
            OM_HOME_SCREEN, OM_RESIGN, OM_OFFER_DRAW, OM_NEWGAME, OM_FLIP_BOARD,
        };

        // Pointers to board and pieces
        std::unique_ptr<Board> board = std::make_unique<Board>();
        std::unique_ptr<SelectedPiece> selectedPiece = std::make_unique<SelectedPiece>();
        std::unique_ptr<std::vector<std::unique_ptr<Piece>>> teamPieces;
        std::unique_ptr<std::vector<std::unique_ptr<Piece>>> oppPieces;
        //std::unique_ptr<std::vector<std::shared_ptr<Piece>>> allPieces;

        // Stockfish
        std::unique_ptr<StockfishManager> sfm = nullptr;

        // Turn management
        bool usersTurn;

    public:
        explicit GameScreen(char _teamID);

        // Game setup
        void SetUpBoard();
        void SetUpPieces();
        void SetupEngine(bool _limitStrength, int _elo, int _level);

        // Display
        bool CreateTextures() override;
        bool Display() override;
        void ResizeScreen() override;

        // Handle events
        void HandleEvents() override;
        void CheckButtons() override;
        std::string FetchOpponentMove();
        std::string FetchOpponentMoveEngine(const std::vector<std::unique_ptr<Piece>>& _teamPieces,
                                            const std::vector<std::unique_ptr<Piece>>& _oppPieces);
};


#endif //CHESS_WITH_SDL_GAMESCREEN_H
