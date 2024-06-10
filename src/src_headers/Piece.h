//
// Created by cew05 on 19/04/2024.
//

#ifndef CHESS_WITH_SDL_PIECE_H
#define CHESS_WITH_SDL_PIECE_H

#include <vector>
#include <string>
#include <SDL_image.h>
#include <algorithm>

#include "GlobalSource.h"
#include "GlobalVars.h"
#include "Board.h"

/*
 * TEMP DEFS
 */

class Board;
class Piece;

/*
 * FULL DEFS
 */

struct Piece_Info {
    std::string name = "Pawn";
    char pieceID = 'a';
    std::string color = "White";
    char colID = 'W';
    Position<char, int> gamepos {'A', 7};
    Position<char, int> lastpos {'A', 7};
};

class AvailableMove{
    private:
        Position<char, int> position {};
        Piece* target = nullptr;

    public:
        void SetPosition(Position<char, int> _position) { position = _position; }
        void SetTarget(Piece* _target) {
            target = _target;
        }
        void GetPosition(Position<char, int> *_assignablePosition) const {
            _assignablePosition->x = position.x;
            _assignablePosition->y = position.y;
        }
        Position<char, int> GetPosition() {
            return position;
        }
        void GetTarget(Piece* _assignableTarget) const {
            _assignableTarget = target;
        }
        Piece* GetTarget() {
            return target;
        }
};

class Piece {
    protected:
        // Game mechanics / position
        std::vector<AvailableMove> validMoves {};
        bool captured = false;
        bool updatedMoves = false;
        bool updatedNextMoves = false;

        // Piece identification
        Piece_Info* info {};

        // SDL display
        std::string imgPath {};
        SDL_Texture* pieceTexture {};
        SDL_Texture* moveHighlights[3] {};
        SDL_Rect pieceRect {};
        SDL_Rect boardposRect {};
        float b_width = 0.1;

        // pawn movements
        bool hasMoved = false;
        bool canPassant = false;
        int passantTimer = 0;
        bool canPromote = false;
        bool promoted = false;
        Piece* promotedTo = nullptr;
        int dir = 1;

        // user input detection
        bool selected = false;
        bool followMouse = false;

    public:
        // Constructor
        Piece(const std::string& _name, const std::string& _color, Position<char, int> _target);

        /*
         * DISPLAY
         */
        // Creating textures
        int CreateTextures();
        void GetRectOfBoardPosition(const Board& board);

        // Displaying piece / moves
        void DisplayPiece();
        void DisplayMoves(const Board& board);

        /*
         * Fetching and testing Moves
         */

        // Fetching and testing Moves
        virtual void FetchMoves(const std::vector<Piece*> &_teamPieces, const std::vector<Piece*> &_oppPieces, const Board& _board);
        void EnforceBorderOnMoves();
        static bool MoveLeadsToCheck(const std::vector<Piece *> &_teamPieces, const std::vector<Piece*> &_oppPieces, const Board& _board, AvailableMove* _move, Piece* _movingPiece);
        void PreventMoveIntoCheck(const std::vector<Piece *> &_teamPieces, const std::vector<Piece *> &_oppPieces, const Board &_board);

        // Clear moves
        void ClearMoves();
        void ClearNextMoves();

        /*
         * TARGETING INFO / POSITIONAL INFO FETCHING
         */

        // Target / Targeting Status Fetching
        bool IsTargetingPosition(Position<char, int> _targetPosition);
        bool IsCheckingKing();

        // Fetching piece if on a particular position
        static Piece* GetTeamPieceOnPosition(const std::vector<Piece*> &_teamPieces, Position<char, int> _targetPos);
        static Piece* GetOppPieceOnPosition(const std::vector<Piece*> &_oppPieces, Position<char, int> _targetPos);
        static Piece* GetPieceOnPosition(const std::vector<Piece*> &_teamPieces, const std::vector<Piece*> &_oppPieces, Position<char, int> _targetPos);

        /*
         * MAKING A MOVE
         */

        // Making a move / Testing a move
        void MoveTo(Position<char, int> _movepos, const Board& _board);
        virtual void UpdateCheckerVars();
        void Captured(bool captured = true);
        void TempMoveTo(AvailableMove* _tempmove);
        void UnMove(AvailableMove* _tempmove);

        // Promotions
        bool ReadyToPromote();
        void UpdatePromoteInfo(Piece* _promotedTo);
        Piece* GetPromotedTo() const;

        /*
         * SELECTING PIECES
         */

        // Selecting Piece
        bool CheckClicked();
        bool UpdateSelected();
        void UnselectPiece();

        /*
         * GETTERS
         */
        Piece_Info* GetPieceInfoPtr() { return info; };
        std::vector<AvailableMove>* GetAvailableMovesPtr() { return &validMoves; };
        bool HasMoved() const { return hasMoved; };
        bool IsCaptured() const { return captured; };
};


#endif //CHESS_WITH_SDL_PIECE_H
