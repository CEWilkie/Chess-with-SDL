//
// Created by cew05 on 28/04/2024.
//

#ifndef CHESS_WITH_SDL_SELECTEDPIECE_H
#define CHESS_WITH_SDL_SELECTEDPIECE_H

#include "Piece.h"

class SelectedPiece {
    private:
        Piece* selectedPiece = nullptr;
        Piece_Info* info_selectedPiece = nullptr;
        std::vector<Piece*>* teamptr;
        std::vector<Piece*>* oppptr;
        Board* boardptr;
        std::string lastMove;

    public:
        SelectedPiece(std::vector<Piece*>* _teamptr, std::vector<Piece*>* _oppptr, Board* _boardptr);
        void SwapPieceSetPointers();
        void CheckForClicked(std::vector<Piece*>* _teamptr);
        void ChangeSelectedPiece(Piece* _newSelected);
        bool MadeMove(std::vector<Piece*>* _teamptr, const Board &_board);
        void GetMove(std::string& _move);
};


#endif //CHESS_WITH_SDL_SELECTEDPIECE_H
