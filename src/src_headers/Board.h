//
// Created by cew05 on 24/04/2024.
//

#ifndef CHESS_WITH_SDL_BOARD_H
#define CHESS_WITH_SDL_BOARD_H

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include "GlobalSource.h"

class Board{
    private:
        // Display sizes
        static const int rows = 8;
        static const int columns = 8;
        int row_labels[rows] {};
        char col_labels[columns] {};
        int tileWidth {};
        int tileHeight {};
        float tile_borderWidth = 0.1;
        float tile_borderHeight = 0.1;

        // SDL display
        SDL_Texture* tileTextures[2] {};
        SDL_Texture* boardTexture {};
        SDL_Texture* boardBase {};
        SDL_Texture* row_label_textures[rows]{};
        SDL_Texture* col_label_textures[columns]{};
        std::string whitePath {"../Resources/GameBoard/Cream_Tile.png"};
        std::string blackPath {"../Resources/GameBoard/Brown_Tile.png"};
        std::string basePath {"../Resources/GameBoard/Board_Base.png"};
        SDL_Rect boardRect {0, 0};
        SDL_Rect baseRect {0, 0};
        SDL_Rect row_label_rects[rows]{};
        SDL_Rect col_label_rects[columns]{};
        SDL_Surface* surface{};

    public:
        Board();
        void DisplayGameBoard();
        int CreateBoardTexture();

        // Getters
        template<class T>
        void GetTileDimensions(T& w, T& h) const {
            w = T(tileWidth);
            h = T(tileHeight);
        }
        static void GetTileRowsColumns(int& _rows, int& _cols) ;
        void GetBoardBLPosition(int& x, int& y) const;
        void GetTileRectFromPosition(SDL_Rect& rect, Position<char, int> position) const;
        void GetBorderedRectFromPosition(SDL_Rect &rect, Position<char, int> position) const;
        // Setters
};


#endif //CHESS_WITH_SDL_BOARD_H
