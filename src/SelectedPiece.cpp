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

bool SelectedPiece::MadeMove(std::vector<Piece*>* _teamptr, const Board &_board) {
    if (selectedPiece == nullptr) return false;

    auto movesList =  *selectedPiece->GetAvailableMovesPtr();
    if (movesList.empty()) {
        return false;
    }

    AvailableMove* selectedMove = nullptr;

    for (auto move : *selectedPiece->GetAvailableMovesPtr()) {
        // Fetch the rect bounds of the move and fit to within the 80% dimensions
        SDL_Rect moveRect;
        boardptr->GetBorderedRectFromPosition(moveRect, move.GetPosition());

        // test if mouse click is inside the rect
        if (mouse.UnheldClick(moveRect)) {
            selectedMove = &move;
            break;
        }
    }

    // break early if no move selected
    if (selectedMove == nullptr) return false;

    // create ACN movestr from the move
    Piece_Info* pInfo = selectedPiece->GetPieceInfoPtr();
    lastMove = "";
    if (selectedMove->GetTarget() != nullptr || selectedPiece->GetPieceInfoPtr()->name != "Pawn") lastMove = pInfo->pieceID;

    // if two of the same pieces could have moved to the position, add col/row id:
    if (std::any_of(teamptr->begin(), teamptr->end(), [&](Piece* piece){
        if (piece == selectedPiece) return false;
        if (piece->GetPieceInfoPtr()->name != selectedPiece->GetPieceInfoPtr()->name) return false;

        Position<char, int> avmovPos {}, selmovPos {};
        selectedMove->GetPosition(&selmovPos);
        for (auto move : *piece->GetAvailableMovesPtr()) {
            move.GetPosition(&avmovPos);

            if (avmovPos.x == selmovPos.x && avmovPos.y == selmovPos.y) {
                printf("piece: %s", piece->GetPieceInfoPtr()->name.c_str());
                return true;
            }
        }
        return false;
    }))
    {
        // check piece on same column
        if (std::any_of(teamptr->begin(), teamptr->end(), [&](Piece* piece){
            if (piece == selectedPiece) return false;
            return (piece->GetPieceInfoPtr()->gamepos.x == pInfo->gamepos.x);
        })) lastMove += std::to_string(pInfo->gamepos.y);

        // check piece on same row
        if (std::any_of(teamptr->begin(), teamptr->end(), [&](Piece* piece){
            if (piece == selectedPiece) return false;
            return (piece->GetPieceInfoPtr()->gamepos.y == pInfo->gamepos.y);
        })) lastMove += pInfo->gamepos.x;
    };


    if (selectedMove->GetTarget() != nullptr) lastMove += "a";
    lastMove += selectedMove->GetPosition().x + std::to_string(selectedMove->GetPosition().y);

    // move selected piece
    selectedPiece->MoveTo(selectedMove->GetPosition(), *boardptr);
    selectedPiece->ClearMoves();


    // Castling moves and Capturing moves both have a target piece to handle
    auto target = selectedMove->GetTarget();
    if (target != nullptr) {
        // if the move target is on the same team, then its a castling move
        if (target->GetPieceInfoPtr()->colID == selectedPiece->GetPieceInfoPtr()->colID) {
            auto pi = selectedPiece->GetPieceInfoPtr();

            // rook must now swap to the opposite side of the king
            int dx = (target->GetPieceInfoPtr()->gamepos.x < pi->gamepos.x) ? 1 : -1;
            target->MoveTo({char(pi->gamepos.x + dx), pi->gamepos.y}, _board);
        } else {
            //move is to capture a target, mark target as captured
            selectedMove->GetTarget()->Captured(true);
        }

    }

    // unselect selected piece and set to lastmoved Piece
    selectedPiece->UnselectPiece();
    selectedPiece = nullptr;
    return true;
}

void SelectedPiece::GetMove(std::string &_move) {
    _move = lastMove;
}
