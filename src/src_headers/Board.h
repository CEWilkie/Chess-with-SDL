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
#include <chrono>
#include "GlobalSource.h"
#include "Piece.h"

class Piece;
class Board;

class Board{
    private:
        // rows, columns on board
        static const int rows = 8;
        static const int columns = 8;
        int row_labels[rows] {};
        char col_labels[columns] {};
        int tileWidth {};
        int tileHeight {};
        float tile_borderWidth = 0.1;
        float tile_borderHeight = 0.1;

        // minimum dimensions of square board
        const int minBoardSize = 500;
        const int minMenuWidth = 75;
        const int minInfoWidth = 300;

        // Rects to retain size/position info of each board section
        SDL_Rect menuRect = {0, 0, minMenuWidth, minBoardSize};
        SDL_Rect boardRect = {minMenuWidth, 0, minBoardSize, minBoardSize};
        SDL_Rect gameInfoRect = {minMenuWidth+minBoardSize, 0, minInfoWidth, minBoardSize};

        // SDL display
        SDL_Texture* tileTextures[2] {};
        SDL_Texture* boardTexture {};
        SDL_Texture* boardBases[2] {};
        SDL_Texture* row_label_textures[rows]{};
        SDL_Texture* col_label_textures[columns]{};

        std::string whitePath {"../Resources/GameBoard/Cream_Tile.png"};
        std::string blackPath {"../Resources/GameBoard/Brown_Tile.png"};
        std::string basePath {"../Resources/GameBoard/Board_Base.png"};
        std::string secondBasePath {"../Resources/GameBoard/Secondary_Base.png"};

        SDL_Rect row_label_rects[rows]{};
        SDL_Rect col_label_rects[columns]{};
        SDL_Surface* surface{};

        // Gameplay recording vars
        std::string moveListFile;
        std::string startPosFile;
        int numEots = 0;
        int turn = 1;

    public:
        Board();
        void DisplayGameBoard();
        int CreateBoardTexture();

        // Getters
        template<class T>
        void GetTileDimensions(T& _w, T& _h) const {
            _w = T(tileWidth);
            _h = T(tileHeight);
        }
        template<class T>
        void GetMinDimensions(T& _w, T& _h) const {
            _w = T(minBoardSize + minInfoWidth + minMenuWidth);
            _h = T(minBoardSize);
        }

        static void GetTileRowsColumns(int& _rows, int& _cols) ;
        void GetBoardBLPosition(int& x, int& y) const;
        void GetTileRectFromPosition(SDL_Rect& rect, Position<char, int> position) const;
        void GetBorderedRectFromPosition(SDL_Rect &rect, Position<char, int> position) const;

        // Setters

        // Gameplay Recording
        bool CreateTempGameFile();
        bool WriteStartPositionsToFile(const std::vector<Piece*> &_allPieces);
        bool WriteMoveToFile(const std::string& _move);
        void IncrementTurn();
};


#endif //CHESS_WITH_SDL_BOARD_H
