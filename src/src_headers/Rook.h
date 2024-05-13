//
// Created by cew05 on 09/05/2024.
//

#ifndef CHESS_WITH_SDL_ROOK_H
#define CHESS_WITH_SDL_ROOK_H

#include "Piece.h"

class Rook : public Piece{
    private:

    public:
        Rook(const std::string& _name, const std::string& _color, Position<char, int> _gamepos);
        void FetchMoves(const std::vector<Piece*> &_teamPieces, const std::vector<Piece*> &_oppPieces, const Board& _board) override;
};


#endif //CHESS_WITH_SDL_ROOK_H
