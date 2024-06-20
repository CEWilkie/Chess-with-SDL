//
// Created by cew05 on 24/04/2024.
//

#ifndef CHESS_WITH_SDL_BOARD_H
#define CHESS_WITH_SDL_BOARD_H

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <filesystem>

#include "GlobalResources.h"
#include "GlobalSource.h"
#include "Piece.h"
#include "ResourceManagers.h"

/*
 * local temp defs
 */

class Piece;

/*
 * Main Defs
 */

class Board{
    private:
        // Game Board Dimensions / info
        static const int rows = 8;
        static const int columns = 8;

        // Tile inside border size vars
        float tile_borderWidth = 0.1;
        float tile_borderHeight = 0.1;
        float boardBorder = 0.1;

        // minimum dimensions of square board widths
        const int minBoardWidth = 500;
        const int minMenuWidth = 150;
        const int minInfoWidth = 300;

        // Local resource manager pointers
        enum RectID : int;
        GenericManager<SDL_Rect>* rm = new GenericManager<SDL_Rect>;

        // Gameplay recording vars
        std::string gameDataDirPath = "../GameData";
        std::string moveListFilePath;
        std::string startPosFilePath;
        std::string timeFormat = "%d_%m_%Y_%H_%M_%S";
        std::string timeStringFormat = "dd_mm_yyyyThh:mm:ssZ";
        int numEots = 0;
        int turn = 1;

    public:
        Board();
        int CreateBoardTexture();
        void DisplayGameBoard();
        bool CreatePromoMenuTexture();
        void DisplayPromoMenu(Piece* _promotingPiece);
        std::string GetPromoMenuInput();

        // Getters
        void GetTileDimensions(int& _w, int& _h) const;
        void GetMinDimensions(int& _w, int& _h) const;
        static void GetRowsColumns(int& _rows, int& _cols);
        static Pair<int> GetRowsColumns() ;
        void GetBoardBLPosition(int& _x, int& _y) const;
        void GetTileRectFromPosition(SDL_Rect& rect, Position<char, int> position) const;
        void GetBorderedRectFromPosition(SDL_Rect &_rect, Position<char, int> _position) const;

        // Setters
        void FillToBounds(int _w, int _h);

        // Gameplay Recording
        bool GameDataDirectoryExists();
        void ClearExcessGameFiles();
        bool CreateGameFiles();
        bool WriteStartPositionsToFile(const std::vector<Piece*> &_allPieces);
        bool WriteMoveToFile(const std::string& _move);
        void IncrementTurn();
};

/*
 * Local Enums
 */

enum Board::RectID : int {
    BOARD, OPTIONS, GAME_INFO, TILE, PROMO_MENU,
    PROMO_QUEEN, PROMO_ROOK, PROMO_BISHOP, PROMO_KNIGHT,
};

#endif //CHESS_WITH_SDL_BOARD_H
