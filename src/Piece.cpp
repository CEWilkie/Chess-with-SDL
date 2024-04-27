//
// Created by cew05 on 19/04/2024.
//

#include "src_headers/Piece.h"

Piece::Piece(const std::string& _name, const std::string& _color, Position<char, int> _gamepos) {
    // set piece info values
    info = {_name, _color, _color[0]};

    // Set path to png file of piece
    // Eg: Knight_White.png. the relevant image files are named as such.
    imgName = "../Resources/" + info.name + "/" + info.name + "_" + info.color + ".png";

    // Create Texture of Piece
    pieceTexture = IMG_LoadTexture(window.renderer, imgName.c_str());

    // Set initial game board position
    gamepos = _gamepos;

    // If white, move up board. If black, move down board:
    dir = (info.col == 'W') ? 1 : -1;
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
    float tileWidth;
    board.GetTileDimensions(tileWidth, tileWidth);

    pieceRect = {0, 0, 0, 0};

    // apply 10 border width buffer (hence set to 80% dimensions)
    pieceRect.w = pieceRect.h = int(tileWidth * (1 - b_width * 2));

    // now determine rect position from game position:
    board.GetBoardTLPosition(pieceRect.x, pieceRect.y);
    pieceRect.x += int(tileWidth * float(gamepos.x-'A'));
    pieceRect.x += int(b_width*tileWidth);

    pieceRect.y += int(tileWidth * float(8 - gamepos.y));
    pieceRect.y += int(b_width*tileWidth);

    boardpos_rect = pieceRect;
}

void Piece::DisplayPiece() {
    if(selected) {
        SDL_RenderCopy(window.renderer, moveHighlights[0], nullptr, &boardpos_rect);
    }

    SDL_RenderCopy(window.renderer, pieceTexture, nullptr, &pieceRect);
}

void Piece::FetchMoves(const std::vector<Piece*> &opp_pieces, const Board& board) {
    /*
     * These moves apply to the Pawn piece. This function is overwritten by other pieces. Move Summary:
     * Forward by 1 always unless blocked
     * Check for takes on side
     * Check for en passant
     * Move 2 if not yet moved
     */

    // Empty movepositions vector
    valid_moves.clear();

    // check that no pieces are blocking the next tile in direction pawn is moving
    if(!std::any_of(opp_pieces.begin(), opp_pieces.end(), [&](Piece* piece){
        return (piece->gamepos.x == gamepos.x && piece->gamepos.y == gamepos.y + dir);
    })) {
        valid_moves.push_back({{gamepos.x, gamepos.y + dir}, false});
    }

    // check for moving forwards 2 spaces
    if (!hasMoved) {
        // check that no pieces are blocking the second tile in direction pawn is moving
        if(!std::any_of(opp_pieces.begin(), opp_pieces.end(), [&](Piece* piece){
            return (piece->gamepos.x == gamepos.x && piece->gamepos.y == gamepos.y + (2*dir));
        })) {
            valid_moves.push_back({{gamepos.x, gamepos.y + dir}, false});
        }
    }

    // check for piece on sides
    for (Piece* piece : opp_pieces) {
        // check left and right side
        if (piece->gamepos.x == gamepos.x - 1 || piece->gamepos.x == gamepos.x + 1){
            // check in front
            if (piece->gamepos.y == gamepos.y + dir) {
                valid_moves.push_back({piece->gamepos, true});
            }

            // check passant
            if (piece->gamepos.y == gamepos.y && canPassant) {
                valid_moves.push_back({piece->gamepos, false});
            }
        }
    }
}

void Piece::DisplayMoves(const Board& board) {
    /*
     * When the piece is selected, will display the valid moves listed in the valid_moves vector
     */

    if (!selected) return;

    for (AvailableMove move : valid_moves) {
        SDL_Rect moveRect = {0, 0};
        board.GetTileDimensions(moveRect.w, moveRect.h);
        Position<int, int> board_topleft {};
        board.GetBoardTLPosition(board_topleft.x, board_topleft.y);

        // convert char position into int and set x, y position of rect
        moveRect.x = board_topleft.x + (int(move.position.x - 'A') * moveRect.w);
        moveRect.y = board_topleft.y + move.position.y * moveRect.h;

        int texture_index = (!move.capture) ? 1 : 2;
        SDL_RenderCopy(window.renderer, moveHighlights[texture_index], nullptr, &moveRect);
    }
}

bool Piece::IsClicked() {
    // function checked on mousedown, so piece remains selected untill next user click
    if (mouseInput.active) {
        selected = mouseInput.InRect(pieceRect);
    }

    // whilst mouse is being held down and piece is selected
    if(mouseInput.active && selected) {
        pieceRect.x = mouseInput.position.x - pieceRect.w/2;
        pieceRect.y = mouseInput.position.y - pieceRect.h/2;
    }
    else {
        pieceRect = boardpos_rect;
    }

    return selected;
}
