//
// Created by cew05 on 09/05/2024.
//

#ifndef CHESS_WITH_SDL_BISHOP_H
#define CHESS_WITH_SDL_BISHOP_H

#include "Piece.h"

class Bishop : public Piece{
    private:

    public:
        Bishop(const std::string& _name, char _colID, std::pair<char, int> _gamepos);
        void FetchMoves(const std::vector<Piece*> &_teamPieces, const std::vector<Piece*> &_oppPieces, const Board& _board) override;
};


#endif //CHESS_WITH_SDL_BISHOP_H
