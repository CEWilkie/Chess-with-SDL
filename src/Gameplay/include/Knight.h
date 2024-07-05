//
// Created by cew05 on 26/04/2024.
//

#ifndef CHESS_WITH_SDL_KNIGHT_H
#define CHESS_WITH_SDL_KNIGHT_H

#include "Piece.h"

class Piece;

class Knight : public Piece{
    private:

    public:
        Knight(char _colID);
        void FetchMoves(const std::vector<std::unique_ptr<Piece>> &_teamPieces,
                        const std::vector<std::unique_ptr<Piece>> &_oppPieces,
                        const std::unique_ptr<Board>& _board) override;
};


#endif //CHESS_WITH_SDL_KNIGHT_H
