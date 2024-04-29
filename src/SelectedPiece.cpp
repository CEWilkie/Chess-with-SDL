//
// Created by cew05 on 28/04/2024.
//

#include "src_headers/SelectedPiece.h"

SelectedPiece::SelectedPiece(std::vector<Piece*>* _teamptr, std::vector<Piece*>* _oppptr, Board* _boardptr) {
    teamptr = _teamptr;
    oppptr = _oppptr;
    boardptr = _boardptr;
}

void SelectedPiece::SwapPieceSetPointers() {
    std::swap(teamptr, oppptr);
}

void SelectedPiece::ChangeSelectedPiece(Piece *_newSelected) {
    if (selectedPiece != nullptr) {
        selectedPiece->UpdateSelected();
        info_selectedPiece = nullptr;
    }

    if ((selectedPiece = _newSelected) != nullptr) {
        selectedPiece->UpdateSelected();
        selectedPiece->FetchMoves(*teamptr, *oppptr, *boardptr);
        info_selectedPiece = selectedPiece->GetPieceInfoPtr();
    }
}

bool SelectedPiece::MadeMove(std::vector<Piece*>* _oppptr) {
    if (selectedPiece == nullptr) return false;

    auto movesList =  *selectedPiece->GetAvailableMovesPtr();
    if (movesList.empty()) {
        printf("empty");
        return false;
    }

    for (auto move : *selectedPiece->GetAvailableMovesPtr()) {
        // Fetch the rect bounds of the move and fit to within the 80% dimensions
        SDL_Rect moveRect;
        boardptr->GetBorderedRectFromPosition(moveRect, move.position);

        // test if mouse click is inside the rect
        if (mouseInput.InRect(moveRect)) {
            // move selected piece
            selectedPiece->MoveTo(move.position, *boardptr);
            selectedPiece->ClearMoves();

            //if move is to capture a target, mark target as captured
            if (move.capture) move.target->Captured();

            // unselect selected piece
            selectedPiece->UnselectPiece();
            selectedPiece = nullptr;
            return true;
        }
    }

    return false;
}
