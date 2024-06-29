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
        Knight(const std::string& _name, char _colID, std::pair<char, int> _gamepos);
        void FetchMoves(const std::vector<Piece*> &_teamPieces, const std::vector<Piece*> &_oppPieces, const Board& _board) override;
};


#endif //CHESS_WITH_SDL_KNIGHT_H
