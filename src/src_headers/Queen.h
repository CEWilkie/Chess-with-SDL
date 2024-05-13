//
// Created by cew05 on 09/05/2024.
//

#ifndef CHESS_WITH_SDL_QUEEN_H
#define CHESS_WITH_SDL_QUEEN_H

#include "Piece.h"
#include "Rook.h"
#include "Bishop.h"

class Queen : public Piece{
    private:

    public:
        Queen(const std::string& _name, const std::string& _color, Position<char, int> _gamepos);
        void FetchMoves(const std::vector<Piece*> &_teamPieces, const std::vector<Piece*> &_oppPieces, const Board& _board) override;
};


#endif //CHESS_WITH_SDL_QUEEN_H
