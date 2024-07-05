//
// Created by cew05 on 09/05/2024.
//

#ifndef CHESS_WITH_SDL_ROOK_H
#define CHESS_WITH_SDL_ROOK_H

#include "Piece.h"

class Rook : public Piece{
    private:
        bool canCastle = true;

    public:
        Rook(char _colID);
        void FetchMoves(const std::vector<std::unique_ptr<Piece>> &_teamPieces,
                        const std::vector<std::unique_ptr<Piece>> &_oppPieces,
                        const std::unique_ptr<Board>& _board) override;
        void UpdateCheckerVars() override;
};


#endif //CHESS_WITH_SDL_ROOK_H
