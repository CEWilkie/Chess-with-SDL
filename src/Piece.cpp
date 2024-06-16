//
// Created by cew05 on 19/04/2024.
//

#include "src_headers/Piece.h"

Piece::Piece(const std::string& _name, char _colID, Position<char, int> _gamepos) {
    // set piece info values
    info = new Piece_Info{_name, (char)tolower(_gamepos.x), _colID, _gamepos};
    if (_name != "Pawn") info->pieceID = _name[0];
    if (_name == "Knight") info->pieceID = 'N';

    // Load TextureID for piece
    TextureID t;
    switch (info->pieceID) {
        case 'K': t = WHITE_KING; break;
        case 'Q': t = WHITE_QUEEN; break;
        case 'R': t = WHITE_ROOK; break;
        case 'B': t = WHITE_BISHOP; break;
        case 'N': t = WHITE_KNIGHT; break;
        default: t = WHITE_PAWN; break;
    }
    info->textureID = TextureID(t + (info->colID == 'W' ? 0 : 1) + PIECE_STYLE.second);
    tm->OpenTexture(info->textureID);

    // Load texture for move display
    tm->OpenTexture(MOVE);
    tm->OpenTexture(CAPTURE);
    tm->OpenTexture(SELECTED);

    // Create rects
    rm->NewResource({0, 0, 0, 0}, Rect::PIECE_RECT);
    rm->NewResource({0, 0, 0, 0}, Rect::BOARDPOS_RECT);


    // Pawn info, direction of movement, and canPromote status
    dir = (info->colID == 'W') ? 1 : -1;
    canPromote = (info->name == "Pawn");
}

/*
 *  DISPLAY
 */

// Creating Textures

int Piece::CreateTextures() {
    /*
     * Create the textures for move highlights, the piece model
     */
    return 0;
}

void Piece::GetRectOfBoardPosition(const Board& board) {
    SDL_Rect rect = rm->FetchResource(Rect::BOARDPOS_RECT);

    board.GetBorderedRectFromPosition(rect, info->gamepos);
    rm->UpdateExistingResource(rect, Rect::BOARDPOS_RECT);
}

// Displaying Piece / Moves

void Piece::DisplayPiece() {
    // Don't display piece if captured
    if (captured) return;

    // temp vars
    SDL_Texture* tempTexture;
    SDL_Rect tempRect;

    if(selected) {
        tempTexture = tm->FetchTexture(SELECTED);
        tempRect = rm->FetchResource(Rect::BOARDPOS_RECT);
        SDL_RenderCopy(window.renderer, tempTexture, nullptr, &tempRect);
    }

    // move to mouse whilst the mousdown is held from initial click
    if(followMouse) {
        tempRect = rm->FetchResource(Rect::PIECE_RECT);
        tempRect.x = mouse.GetMousePosition().x - tempRect.w / 2;
        tempRect.y = mouse.GetMousePosition().y - tempRect.h / 2;
    }
    else tempRect = rm->FetchResource(Rect::BOARDPOS_RECT);
    rm->UpdateExistingResource(tempRect, Rect::PIECE_RECT);

    tempTexture = tm->FetchTexture(info->textureID);
    tempRect = rm->FetchResource(Rect::PIECE_RECT);
    SDL_RenderCopy(window.renderer, tempTexture, nullptr, &tempRect);
}

void Piece::DisplayMoves(const Board& board) {
    /*
     * When the piece is selected, will display the valid moves listed in the validMoves vector
     */

    if (!selected || captured) return;

    for (AvailableMove move : validMoves) {
        SDL_Rect moveRect = {0, 0};
        board.GetBorderedRectFromPosition(moveRect, move.GetPosition());

        // if the move is a capture (except if capturing same team rook as this is a castling move) then use diff icon
        SDL_Texture* moveIcon = tm->FetchTexture(MOVE);
        if (move.GetTarget() != nullptr) {
            if (move.GetTarget()->GetPieceInfoPtr()->colID != info->colID) {
                moveIcon = tm->FetchTexture(CAPTURE);
            }
        }

        SDL_RenderCopy(window.renderer, moveIcon, nullptr, &moveRect);
    }
}

/*
 * FETCHING AND TESTING MOVES
 */

// Fetching and testing Moves

void Piece::FetchMoves(const std::vector<Piece*> &_teamPieces, const std::vector<Piece*> &_oppPieces, const Board& _board) {
    /*
     * These moves apply to the Pawn piece. This function is overwritten by other pieces. Move Summary:
     * Forward by 1 always unless blocked
     * Check for takes on side
     * Check for en passant
     * Move 2 if not yet moved
     */

    // only fetch moves if the moves have not been updated, and whilst piece is not captured
    if (updatedMoves || captured) return;

    // ensure valid moves is empty
    validMoves.clear();

    // normal moves
    // check that no pieces are blocking the next tile in direction pawn is moving
    if(GetPieceOnPosition(_teamPieces, _oppPieces, {info->gamepos.x, info->gamepos.y + dir}) == nullptr){
        validMoves.emplace_back();
        validMoves.back().SetPosition({info->gamepos.x, info->gamepos.y + dir});

        // check for moving forwards 2 spaces
        if (!hasMoved) {
            // check that no pieces are blocking the second next tile in direction pawn is moving
            if(GetPieceOnPosition(_teamPieces, _oppPieces, {info->gamepos.x, info->gamepos.y + (2 * dir)}) == nullptr){
                validMoves.emplace_back();
                validMoves.back().SetPosition({info->gamepos.x, info->gamepos.y + (2*dir)});
            }
        }
    }

    // check for capture moves
    for (Piece* piece : _oppPieces) {
        // ignore captured pieces
        if (piece->captured) continue;

        // check left and right side
        if (piece->info->gamepos.x == info->gamepos.x - 1 || piece->info->gamepos.x == info->gamepos.x + 1){
            // check in front
            if (piece->info->gamepos.y == info->gamepos.y + dir) {
                validMoves.emplace_back();
                validMoves.back().SetPosition(piece->info->gamepos);
                validMoves.back().SetTarget(piece);
            }

            // check passant
            if (piece->info->gamepos.y == info->gamepos.y && piece->canPassant) {
                validMoves.emplace_back();
                validMoves.back().SetPosition({piece->info->gamepos.x, piece->info->gamepos.y + dir});
                validMoves.back().SetTarget(piece);
            }
        }
    }

    EnforceBorderOnMoves();

    updatedMoves = true;
}

void Piece::EnforceBorderOnMoves() {
    // Removes any moves from the valid moves list that go past the border.
    int rows, cols;
    Board::GetRowsColumns(rows, cols);

    for (auto iter = validMoves.begin(); iter != validMoves.end();)  {
        Position<char, int> movPos {};
        iter->GetPosition(&movPos);

        // check if move exceeds b positions then check a positions
        if ((movPos.y < 1 || movPos.y > rows) ||  (movPos.x < 'a' || movPos.x > char('a' + cols - 1))){
            iter = validMoves.erase(iter);
        }
        else {
            iter++;
        }
    }
}

bool Piece::MoveLeadsToCheck(const std::vector<Piece *> &_teamPieces, const std::vector<Piece *> &_oppPieces,
                             const Board &_board, AvailableMove* _move, Piece* _movingPiece) {
    // store move data locally, as info at pointer can be changed and idk how to fix that
    AvailableMove move = *_move;

    // move piece temporarily and calculate if king is being checked after the move
    _movingPiece->TempMoveTo(&move);

    bool checking = false;
    for (auto oppPiece : _oppPieces) {
        oppPiece->ClearMoves();
        oppPiece->FetchMoves(_oppPieces, _teamPieces, _board);
        // if the oppPiece is checking the king, then the move is invalid
        if (oppPiece->IsCheckingKing()) {
            checking = true;
            break;
        }
    }

    // reset position and moves
    _movingPiece->UnMove(&move);

    for (auto oppPiece : _oppPieces) {
        oppPiece->ClearMoves();
        oppPiece->FetchMoves(_oppPieces, _teamPieces, _board);
    }

    // return checking value
    return checking;
}

void Piece::PreventMoveIntoCheck(const std::vector<Piece *> &_teamPieces, const std::vector<Piece *> &_oppPieces, const Board &_board) {
    if (updatedNextMoves) return;

    for (auto move : validMoves) {
        Position<char, int> movPos {};
        move.GetPosition(&movPos);
    }

    // track indexes of moves which need to be erased
    int eraseMoveIndexes[validMoves.size()];

    for (int i = 0; i < validMoves.size(); i++) {
        Position<char, int> movPos {};
        validMoves[i].GetPosition(&movPos);
        eraseMoveIndexes[i] = MoveLeadsToCheck(_teamPieces, _oppPieces,
                                               _board, &validMoves[i], this);
    }

    // now remove the marked moves
    int emiIter = 0;
    for (auto iter = validMoves.begin(); iter < validMoves.end();){
        if (eraseMoveIndexes[emiIter] == 1) {
            Position<char, int> movPos {};
            iter->GetPosition(&movPos);
            iter = validMoves.erase(iter);
        } else {
            iter++;
        }

        emiIter++;
    }


    updatedNextMoves = true;
}

// Clear Moves

void Piece::ClearMoves() {
    validMoves.clear();
    updatedMoves = false;
}

void Piece::ClearNextMoves() {
    updatedNextMoves = false;
}

/*
 * TARGETING INFO / POSITIONAL INFO FETCHING
 */

// Target / Targeting Status fetching

bool Piece::IsTargetingPosition(Position<char, int> _targetPosition) {
    // checks if any of the pieces moves are targeting the specified position _targetPosition

    return std::any_of(validMoves.begin(), validMoves.end(), [&](AvailableMove move){
        return (move.GetPosition().x == _targetPosition.x && move.GetPosition().y == _targetPosition.y);
    });
}

bool Piece::IsCheckingKing() {
    if (captured) return false;

    return (std::any_of(validMoves.begin(), validMoves.end(), [](AvailableMove move){
        if (move.GetTarget() == nullptr) return false;

        return (move.GetTarget()->info->pieceID == 'K');
    }));
}

// Fetching piece if on a particular position

Piece* Piece::GetTeamPieceOnPosition(const std::vector<Piece *> &_teamPieces, Position<char, int> _targetPos) {
    for (auto teamPiece : _teamPieces) {
        // ignore captured pieces
        if (teamPiece->captured) continue;

        // check non-captured piece
        if (teamPiece->info->gamepos.x == _targetPos.x && teamPiece->info->gamepos.y == _targetPos.y) {
            return teamPiece;
        }
    }

    return nullptr;
}

Piece* Piece::GetOppPieceOnPosition(const std::vector<Piece *> &_oppPieces, Position<char, int> _targetPos) {
    for (auto oppPiece : _oppPieces) {
        // ignore captured pieces
        if (oppPiece->captured) continue;

        // check non-captured piece
        if (oppPiece->info->gamepos.x == _targetPos.x && oppPiece->info->gamepos.y == _targetPos.y) {
            return oppPiece;
        }
    }

    // no capturable pieces;
    return nullptr;
}

Piece* Piece::GetPieceOnPosition(const std::vector<Piece *> &_teamPieces, const std::vector<Piece *> &_oppPieces, Position<char, int> _targetPos) {
    Piece* piece = GetTeamPieceOnPosition(_teamPieces, _targetPos);
    if (piece != nullptr) return piece;

    piece = GetOppPieceOnPosition(_oppPieces, _targetPos);
    return piece;
}


/*
 * MAKING A MOVE
 */

// Making / Testing a move

void Piece::MoveTo(Position<char, int> _movepos, const Board &_board) {
    // prevent moves when captured
    if  (captured) return;

    // check if distance being moved is 2 forward, and is pawn
    canPassant = false;
    if (info->name == "Pawn" && _movepos.y == info->gamepos.y + (2*dir)) {
        canPassant = true;
        passantTimer = 2;
    }

    // Update game position and movement values
    info->lastpos = info->gamepos;
    info->gamepos = _movepos;
    hasMoved = true;

    // Remake rect
    GetRectOfBoardPosition(_board);
}

void Piece::UpdateCheckerVars() {
    passantTimer -= 1;
    canPassant = passantTimer > 0;
}

void Piece::Captured(bool _captured) {
    // update captured value to prevent piece from being interacted with or displayed.
    captured = _captured;

    // Close textures
    tm->CloseTexture(info->textureID);
    tm->CloseTexture(MOVE);
    tm->CloseTexture(CAPTURE);
    tm->CloseTexture(SELECTED);
}

void Piece::TempMoveTo(AvailableMove* _tempmove) {
    info->lastpos = info->gamepos;
    info->gamepos = _tempmove->GetPosition();

    //make temp capture
    if (_tempmove->GetTarget() != nullptr) {
        _tempmove->GetTarget()->captured = true;
    }
}

void Piece::UnMove(AvailableMove* _tempmove) {
    info->gamepos = info->lastpos;

    //uncapture
    if (_tempmove->GetTarget() != nullptr){
        _tempmove->GetTarget()->captured = false;
    }
}

// Promotions

bool Piece::ReadyToPromote() {
    // Determine if piece at end of column
    int eoc = (info->colID == 'W') ? Board::GetRowsColumns().a : 1;
    return (canPromote && info->gamepos.y == eoc && !captured);
}

void Piece::UpdatePromoteInfo(Piece* _promotedTo) {
    promoted = true;
    promotedTo = _promotedTo;
}

Piece* Piece::GetPromotedTo() const {
    return promotedTo;
}

/*
 * SELECTING PIECES
 */

bool Piece::CheckClicked() {
    if (captured) return false;

    return  mouse.UnheldClick(rm->FetchResource(Rect::BOARDPOS_RECT));
}

bool Piece::UpdateSelected() {
    // prevent selection when captured
    if  (captured) return false;

    // user clicked in region of piece, so piece is selected
    selected = mouse.UnheldClick(rm->FetchResource(Rect::BOARDPOS_RECT));
    return selected;
}

void Piece::UnselectPiece() {
    selected = false;
}
