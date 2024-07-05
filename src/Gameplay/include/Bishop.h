//
// Created by cew05 on 09/05/2024.
//

#ifndef CHESS_WITH_SDL_BISHOP_H
#define CHESS_WITH_SDL_BISHOP_H

#include "Piece.h"

class Bishop : public Piece{
    private:

    public:
        Bishop(char _colID);
        void FetchMoves(const std::vector<std::unique_ptr<Piece>> &_teamPieces,
                        const std::vector<std::unique_ptr<Piece>> &_oppPieces,
                        const std::unique_ptr<Board>& _board) override;
};


#endif //CHESS_WITH_SDL_BISHOP_H
