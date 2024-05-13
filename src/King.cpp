//
// Created by cew05 on 09/05/2024.
//

#include "src_headers/King.h"

King::King(const std::string &_name, const std::string &_color, Position<char, int> _gamepos)
: Piece(_name, _color,_gamepos) {
    info->pieceID = 'K';
}

void King::FetchMoves(const std::vector<Piece *> &_teamPieces, const std::vector<Piece *> &_oppPieces,
                      const Board &_board) {
    /*
     * King moves are in the 8 tiles surrounding him
     * [_][_][_][_][_]
     * [_][x][x][x][_]
     * [_][x][K][x][_]
     * [_][x][x][x][_]
     * [_][_][_][_][_]
     * However, he cannot enter a position where he is being checked (targeted by another piece)
     */

    // only fetch moves if the moves have not been updated, and whilst piece is not captured
    if (updatedMoves || captured) return;

    // ensure valid moves is empty
    validMoves.clear();

    // fetch all moves assuming none result in king being checked
    AvailableMove move;
    for (int dc = -1; dc < 2; dc++) {
        for (int dr = -1; dr < 2; dr++) {
            move.position = {char(info->gamepos.x+dc), info->gamepos.y+dr};
            if (PositionOccupied(_teamPieces, _oppPieces, move.position) == -1) continue;

            Piece* target;
            if ((target = GetOpponentOnPosition(_oppPieces, move.position)) != nullptr) {
                move.capture = true;
                move.target = target;
            }

            validMoves.push_back(move);
        }
    }

    EnforceBorderOnMoves();

    // now test for checks on move made

    updatedMoves = true;
}