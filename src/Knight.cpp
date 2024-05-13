//
// Created by cew05 on 26/04/2024.
//

#include "src_headers/Knight.h"

Knight::Knight(const std::string& _name, const std::string& _color, Position<char, int> _gamepos)
: Piece(_name, _color, _gamepos) {
        info->pieceID = 'N';
}

void Knight::FetchMoves(const std::vector<Piece *> &_teamPieces, const std::vector<Piece *> &_oppPieces,
                        const Board &_board) {
    /*
     * Fetches the moves for
     *  [_][x][_][x][_]
     *  [x][_][_][_][x]
     *  [_][_][K][_][_]
     *  [x][_][_][_][x]
     *  [_][x][_][x][_]
     *  where x marks an empty tile or capturable piece
     */

    // only fetch moves if the moves have not been updated, and whilst piece is not captured
    if (updatedMoves || captured) return;

    // ensure valid moves is empty
    validMoves.clear();

    // fetch moves and assume all to be non-capturing and free spaces
    for (int x = -2; x < 3; x++) {
        if (x==0) continue;
        int vDir = (abs(x) == 2) ? 1 : 2;
        validMoves.push_back({char(info->gamepos.x-x), info->gamepos.y+vDir});
        validMoves.push_back({char(info->gamepos.x-x), info->gamepos.y-vDir});
    }

    // ensure moves are within the A-H and 1-8 bounds
    EnforceBorderOnMoves();

    // check if move is team-occupied or a capture
    for (auto iter = validMoves.begin(); iter < validMoves.end();) {
        // remove team-occupied moves
        if (PositionOccupied(_teamPieces, _oppPieces, iter->position) == -1) {
            iter = validMoves.erase(iter);
            continue;
        }

        // apply capture target to capturing moves
        Piece* target;
        if ((target = GetOpponentOnPosition(_oppPieces, iter->position)) != nullptr) {
            iter->capture = true;
            iter->target = target;
        }

        iter++;
    }

    updatedMoves = true;
}
