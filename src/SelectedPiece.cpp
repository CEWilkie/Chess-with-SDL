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

void SelectedPiece::CheckForClicked(std::vector<Piece*>* _teamptr) {
    // Has the user clicked within the window boundaries
    if (!mouse.UnheldClick(window.currentRect)) return;

    if (!std::any_of(_teamptr->begin(), _teamptr->end(), [&](Piece* piece)
    {
        if (piece->CheckClicked()) {
            // piece was clicked, so update the selected piece
            ChangeSelectedPiece(piece);
            return true;
        }
        return false;
    })) {
        // no piece was clicked
        ChangeSelectedPiece(nullptr);
    }
}

void SelectedPiece::ChangeSelectedPiece(Piece *_newSelected) {
    // clear selectedPiece pointers
    if (selectedPiece != nullptr) {
        selectedPiece->UpdateSelected();
        selectedPiece = nullptr;
        info_selectedPiece = nullptr;
    }

    // set new selectedPiece
    if ((selectedPiece = _newSelected) != nullptr) {
        selectedPiece->UpdateSelected();
        info_selectedPiece = selectedPiece->GetPieceInfoPtr();
    }
}

bool SelectedPiece::MadeMove(std::vector<Piece*>* _oppptr) {
    if (selectedPiece == nullptr) return false;

    auto movesList =  *selectedPiece->GetAvailableMovesPtr();
    if (movesList.empty()) {
        return false;
    }

    for (auto move : *selectedPiece->GetAvailableMovesPtr()) {
        // Fetch the rect bounds of the move and fit to within the 80% dimensions
        SDL_Rect moveRect;
        boardptr->GetBorderedRectFromPosition(moveRect, move.position);

        // test if mouse click is inside the rect
        if (mouse.UnheldClick(moveRect)) {
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
