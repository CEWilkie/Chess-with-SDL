//
// Created by cew05 on 28/04/2024.
//

#ifndef CHESS_WITH_SDL_SELECTEDPIECE_H
#define CHESS_WITH_SDL_SELECTEDPIECE_H

#include "Piece.h"

class SelectedPiece {
    private:
        // currently selected piece info
        Piece* selectedPiece = nullptr;
        Piece_Info* selectedPieceInfo = nullptr;
        AvailableMove* selectedMove = nullptr;

        // last moved piece / move info
        Piece* lastMovedPiece;
        Piece_Info lastMovedInfo;
        AvailableMove lastMove;
        Piece_Info lastMovedTargetInfo;

        // ACN construction
        std::string lastMoveACN;
        std::vector<std::string> moveList {};

    public:
        SelectedPiece();

        // Checking for inputs
        void CheckForPieceClicked(std::vector<Piece*>* _teamptr);
        bool CheckForMoveClicked(const Board* _board);
        void ChangeSelectedPiece(Piece* _newSelected);

        // ACN composing
        void GetACNMoveString(std::string& _move);
        std::string GetACNMoveString() { return lastMoveACN; };
        void CreateACNstring(std::vector<Piece*>* _teamptr);

        // Making a move
        void MakeMove(Board* _board);

        // Getters
        Piece* GetSelectedPiece() { return selectedPiece; };
};


#endif //CHESS_WITH_SDL_SELECTEDPIECE_H
