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
        PieceInfo* selectedPieceInfo = nullptr;
        AvailableMove selectedMove {};

        // last moved piece / move info
        Piece* lastMovedPiece = nullptr;
        PieceInfo lastMovedInfo {};
        AvailableMove lastMove {};
        PieceInfo lastMovedTargetInfo {};

        // ACN construction
        std::string lastMoveACN {};
        std::vector<std::string> moveList {};

    public:
        SelectedPiece();

        // Checking for inputs
        void CheckForPieceClicked(const std::vector<std::unique_ptr<Piece>>& _teamptr);
        bool CheckForMoveClicked(const std::unique_ptr<Board>& _board);
        void ChangeSelectedPiece(const std::unique_ptr<Piece>& _newSelected);

        // ACN composing
        void GetACNMoveString(std::string& _move);
        std::string GetACNMoveString() { return lastMoveACN; };
        void CreateACNstring(const std::vector<std::unique_ptr<Piece>>& _teamptr);

        // Making a move
        void MakeMove(const std::unique_ptr<Board>& _board);
        void MakeMove(Piece* _piece,
                      const AvailableMove& _move,
                      const std::unique_ptr<Board>& _board);

        // Getters
        Piece* GetSelectedPiece() { return selectedPiece; };
};


#endif //CHESS_WITH_SDL_SELECTEDPIECE_H
