//
// Created by cew05 on 24/04/2024.
//

#ifndef CHESS_WITH_SDL_BOARD_H
#define CHESS_WITH_SDL_BOARD_H

#include <SDL_image.h>
#include <vector>
#include <string>
#include "GlobalSource.h"

class Board{
    private:
        //
        static const int rows = 8;
        static const int columns = 8;
        char row_labels[rows] {};
        char col_labels[columns] {};
        int tileWidth {};
        int tileHeight {};

        // SDL display
        SDL_Texture* tileTextures[2] {};
        SDL_Texture* boardTexture {};
        std::string whitePath {"../Resources/GameBoard/creamTile.png"};
        std::string blackPath {"../Resources/GameBoard/brownTile.png"};
        SDL_Rect boardRect {0, 0};

    public:
        Board();
        void DisplayGameBoard();
        int CreateBoardTexture();
};


#endif //CHESS_WITH_SDL_BOARD_H
