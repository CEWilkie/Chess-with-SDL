//
// Created by cew05 on 09/05/2024.
//

#include "src_headers/Rook.h"

Rook::Rook(const std::string &_name, char _colID, Position<char, int> _gamepos)
: Piece(_name, _colID,_gamepos) {
    info->pieceID = 'R';
}

void Rook::FetchMoves(const std::vector<Piece *> &_teamPieces, const std::vector<Piece *> &_oppPieces,
                        const Board &_board) {
    /* Fetches vertical/horizontal empty squares in all 4 directions until either:
     * Exceeding board bounds
     * Reaching tile occupied by teammate
     * Reaching tile occupied by enemy piece (capturable)
     */

    // only fetch moves if the moves have not been updated, and whilst piece is not captured
    if (updatedMoves || captured) return;

    // ensure valid moves is empty
    validMoves.clear();

    // delta rows or change in rows. increments by +-1 to move in diagonals
    int maxRows, maxCols, dr = 0, dc = 0;
    Board::GetRowsColumns(maxRows, maxCols);

    // repeat 4 times for each of the diagonal directions (order ResType -> R -> B -> L)
    for (int direction = 0; direction < 4; direction++) {
        // flip direction to negative value on B, L directions
        int dir = (direction > 1) ? -1 : 1;

        // determine row and column change using direction (directionRow, directionColumn) and dir value
        dr = (direction == 0 || direction == 2) ? dir : 0;
        dc = (direction == 1 || direction == 3) ? dir : 0;

        // set move to next tile in direction
        AvailableMove move;
        Position<char, int> movPos = {char(info->gamepos.x + dc), info->gamepos.y + dr};
        move.SetPosition(movPos);

        while ((0 < movPos.y && movPos.y <= maxRows) && ('a' <= movPos.x && movPos.x <= char('a' + maxCols - 1))) {

            // if the position is occupied by a teammember, do not add move, and change direction
            if (GetTeamPieceOnPosition(_teamPieces, movPos) != nullptr) {
                break;
            }

            // if the position is occupied by an enemy, mark move as a capture, add to list and change direction
            Piece* target;
            if ((target = GetOppPieceOnPosition(_oppPieces, movPos)) != nullptr) {
                move.SetTarget(target);
                validMoves.push_back(move);
                break;
            }

            // add move to valid moves list
            validMoves.push_back(move);

            // next tile in direction
            movPos = {char(movPos.x+dc), movPos.y+dr};
            move.SetPosition(movPos);
        }
    }

    EnforceBorderOnMoves();

    updatedMoves = true;
}

void Rook::UpdateCheckerVars() {
    if (hasMoved) canCastle = true;
}
