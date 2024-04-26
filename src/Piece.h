//
// Created by cew05 on 19/04/2024.
//

#ifndef CHESS_WITH_SDL_PIECE_H
#define CHESS_WITH_SDL_PIECE_H

#include <vector>
#include <string>
#include <SDL_image.h>

#include "GlobalSource.h"
#include "Board.h"

class Piece {
    protected:
        // Game mechanics / position
        Position gamepos {};
        Position rectpos {};
        std::vector<Position> vec_movepos {};

        // SDL display
        std::string imgPath {};
        SDL_Texture* texture {};

    public:
        Piece(const std::string& _imgPath, Position _gamepos);
        void DisplayPiece(Board board);
};


#endif //CHESS_WITH_SDL_PIECE_H
