//
// Created by cew05 on 26/04/2024.
//

#include "include/Knight.h"

Knight::Knight(const std::string& _name, char _colID, Position<char, int> _gamepos)
: Piece(_name, _colID, _gamepos) {

}

void Knight::FetchMoves(const std::vector<Piece *> &_teamPieces, const std::vector<Piece *> &_oppPieces,
                        const Board &_board) {
    /*
     * Fetches the moves for
     *  [_][a][_][a][_]
     *  [a][_][_][_][a]
     *  [_][_][Key][_][_]
     *  [a][_][_][_][a]
     *  [_][a][_][a][_]
     *  where a marks an empty tile or capturable piece
     */

    // only fetch moves if the moves have not been updated, and whilst piece is not captured
    if (updatedMoves || captured) return;

    // ensure valid moves is empty
    validMoves.clear();

    // fetch moves and assume all to be non-capturing and free spaces
    for (int x = -2; x < 3; x++) {
        if (x==0) continue;
        int vDir = (abs(x) == 2) ? 1 : 2;
        validMoves.emplace_back();
        validMoves.back().SetPosition({char(info->gamepos.x-x), info->gamepos.y+vDir});
        validMoves.emplace_back();
        validMoves.back().SetPosition({char(info->gamepos.x-x), info->gamepos.y-vDir});
    }

    // ensure moves are within the A-H and 1-8 bounds
    EnforceBorderOnMoves();

    // check if move is team-occupied or a capture
    for (auto iter = validMoves.begin(); iter < validMoves.end();) {
        // remove team-occupied moves
        if (GetTeamPieceOnPosition(_teamPieces, iter->GetPosition()) != nullptr) {
            iter = validMoves.erase(iter);
            continue;
        }

        // apply capture target to capturing moves
        Piece* target;
        if ((target = GetOppPieceOnPosition(_oppPieces, iter->GetPosition())) != nullptr) {
            iter->SetTarget(target);
        }

        iter++;
    }

    updatedMoves = true;
}
