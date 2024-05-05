//
// Created by cew05 on 19/04/2024.
//

#include "src_headers/Piece.h"

Piece::Piece(const std::string& _name, const std::string& _color, Position<char, int> _gamepos) {
    // set piece info values
    info = new Piece_Info{_name, _color, _color[0]};

    // Set path to png file of piece
    // Eg: Knight_White.png. the relevant image files are named as such.
    imgName = "../Resources/" + info->name + "/" + info->name + "_" + info->color + ".png";

    // Create Texture of Piece
    pieceTexture = IMG_LoadTexture(window.renderer, imgName.c_str());

    // Set initial game board position
    info->gamepos = _gamepos;

    // If white, move up board. If black, move down board. This is only necessary for pawns
    dir = (info->colID == 'W') ? 1 : -1;
}

int Piece::CreateTextures() {
    if ((moveHighlights[0] = IMG_LoadTexture(window.renderer, "../Resources/Piece/Selected_Target.png")) == nullptr) {
        LogError("Failed to create selected texture", SDL_GetError(), false);
        return -1;
    }
    if ((moveHighlights[1] = IMG_LoadTexture(window.renderer, "../Resources/Piece/Available_Space.png")) == nullptr) {
        LogError("Failed to create selected texture", SDL_GetError(), false);
        return -1;
    }
    if ((moveHighlights[2] = IMG_LoadTexture(window.renderer, "../Resources/Piece/Attacking_Piece.png")) == nullptr) {
        LogError("Failed to create selected texture", SDL_GetError(), false);
        return -1;
    }

    if ((pieceTexture = IMG_LoadTexture(window.renderer, imgName.c_str())) == nullptr) {
        LogError("Failed to create piece pieceTexture", SDL_GetError(), false);
        return -1;
    }

    return 0;
}

void Piece::GetRectOfBoardPosition(const Board& board) {
    board.GetBorderedRectFromPosition(boardposRect, info->gamepos);
    pieceRect = boardposRect;
}

void Piece::DisplayPiece() {
    // Don't display piece if captured
    if (captured) return;

    if(selected) {
        SDL_RenderCopy(window.renderer, moveHighlights[0], nullptr, &boardposRect);
    }

    // move to mouse whilst the mousdown is held from initial click
    if(followMouse) {
        pieceRect.x = mouse.GetMousePosition().x - pieceRect.w / 2;
        pieceRect.y = mouse.GetMousePosition().y - pieceRect.h / 2;
    }
    else {
        pieceRect = boardposRect;
    }

    SDL_RenderCopy(window.renderer, pieceTexture, nullptr, &pieceRect);
}

int Piece::PieceOnPosition(const std::vector<Piece *> &_teamPieces, const std::vector<Piece *> &_oppPieces, Position<char, int> _targetPos) {
    /*
     * Function checks the target position and returns a value depending upon if there is a piece on the tile, if its
     * an enemy piece, or if the tile is empty.
     * EMPTY RETURNS 0; ENEMY RETURNS 1; TEAM RETURNS -1;
     */

    // Check if team is on position
    if (std::any_of(_teamPieces.begin(), _teamPieces.end(), [&](Piece* piece){
        // ignore captured pieces
        if (piece->captured) return false;

        // check non-captured piece
        return (piece->info->gamepos.x == _targetPos.x && piece->info->gamepos.y == _targetPos.y);
    })) {
        return -1;
    }

    // Check if enemy is on position
    if (std::any_of(_oppPieces.begin(), _oppPieces.end(), [&](Piece* piece){
        // ignore captured pieces
        if (piece->captured) return false;

        // check non-captured piece
        return (piece->info->gamepos.x == _targetPos.x && piece->info->gamepos.y == _targetPos.y);
    })) {
        return 1;
    }

    // since no team or enemy on position, position is empty.
    return 0;
}

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
    if(PieceOnPosition(_teamPieces, _oppPieces, {info->gamepos.x, info->gamepos.y + dir}) == 0){
        validMoves.push_back({{info->gamepos.x, info->gamepos.y + dir}, false});

        // check for moving forwards 2 spaces
        if (!hasMoved) {
            // check that no pieces are blocking the second next tile in direction pawn is moving
            if(PieceOnPosition(_teamPieces, _oppPieces, {info->gamepos.x, info->gamepos.y+(2*dir)}) == 0){
                validMoves.push_back({{info->gamepos.x, info->gamepos.y + (2*dir)}, false});
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
                validMoves.push_back({piece->info->gamepos, true, piece});
            }

            // check passant
            if (piece->info->gamepos.y == info->gamepos.y && piece->canPassant) {
                validMoves.push_back({{piece->info->gamepos.x, piece->info->gamepos.y + dir}, true, piece});
            }
        }
    }

    updatedMoves = true;
}

void Piece::EnforceBorderOnMoves() {
    // Removes any moves from the valid moves list that go past the border.
    int rows, cols;
    Board::GetTileRowsColumns(rows, cols);

    for (auto iter = validMoves.begin(); iter != validMoves.end();)  {
        // check if move exceeds y positions then check x positions
        if ((iter->position.y < 1 || iter->position.y > rows) ||
        (iter->position.x < 'A' || iter->position.x > char('A' + cols))){
            validMoves.erase(iter);
        }
        else {
            iter++;
        }
    }
}

void Piece::ClearMoves() {
    validMoves.clear();
    updatedMoves = false;
}

void Piece::DisplayMoves(const Board& board) {
    /*
     * When the piece is selected, will display the valid moves listed in the validMoves vector
     */

    if (!selected || captured) return;

    for (AvailableMove move : validMoves) {
        SDL_Rect moveRect = {0, 0};
        board.GetBorderedRectFromPosition(moveRect, move.position);

        int texture_index = (!move.capture) ? 1 : 2;
        SDL_RenderCopy(window.renderer, moveHighlights[texture_index], nullptr, &moveRect);
    }
}

bool Piece::CheckClicked() {
    return  mouse.UnheldClick(boardposRect);
}

bool Piece::UpdateSelected() {
    // prevent selection when captured
    if  (captured) return false;

    // user clicked in region of piece, so piece is selected
    selected = mouse.UnheldClick(boardposRect);
    return selected;
}

void Piece::UnselectPiece() {
    selected = false;
}

void Piece::MoveTo(Position<char, int> _movepos, const Board &_board) {
    // prevent moves when captured
    if  (captured) return;

    // check if distance being moved is 2 forward, and is pawn
    canPassant = false;
    if (info->name == "Pawn" && _movepos.y == info->gamepos.y + (2*dir)) {
        canPassant = true;
    }

    // Update game position and movement values
    info->gamepos = _movepos;
    hasMoved = true;

    // Remake rect
    GetRectOfBoardPosition(_board);
}

void Piece::Captured() {
    // update captured value to prevent piece from being interacted with or displayed.
    captured = true;
}
