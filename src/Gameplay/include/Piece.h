//
// Created by cew05 on 19/04/2024.
//

#ifndef CHESS_WITH_SDL_PIECE_H
#define CHESS_WITH_SDL_PIECE_H

#include <vector>
#include <string>
#include <algorithm>
#include <SDL_image.h>
#include <memory>

#include "../../src_headers/GlobalSource.h"
#include "../../src_headers/GlobalResources.h"
#include "Board.h"

/*
 * TEMP DEFS
 */

class Board;
class Piece;

/*
 * FULL DEFS
 */

struct PieceInfo {
    std::string name = "Pawn";
    char pieceID = 'a';
    char colID = 'W';
    std::pair<char, int> gamepos {'A', 7};
    std::pair<char, int> lastpos {'A', 7};
    TextureID textureID = WHITE_PAWN;
};

class AvailableMove{
    private:
        std::pair<char, int> position {};
        Piece* target = nullptr;

    public:
        void SetPosition(std::pair<char, int> _position) { position = _position; }
        void SetTarget(Piece* _target) {
            target = _target;
        }
        void GetPosition(std::pair<char, int> *_assignablePosition) const {
            _assignablePosition->first = position.first;
            _assignablePosition->second = position.second;
        }
        [[nodiscard]] std::pair<char, int> GetPosition() const {
            return position;
        }
        [[nodiscard]] Piece* GetTarget() const {
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
        std::unique_ptr<PieceInfo> info;

        // Local ResourceManager pointers
        enum RectID : int;
        GenericManager<SDL_Rect>* rm = new GenericManager<SDL_Rect>;

        // SDL display
        std::string imgPath {};

        // pawn movements
        bool hasMoved = false;
        bool justMoved = false;
        int lastMoveDisplayTimer = 0;
        bool canPassant = false;
        int passantTimer = 0;
        int animLenTicks = 200;
        Uint64 animStartTick = 0;
        bool canPromote = false;
        bool promoted = false;
        Piece* promotedTo = nullptr;
        int dir = 1;

        // Castling for King
        bool canCastleKingside = false;
        bool canCastleQueenside = false;

        // user input detection
        bool selected = false;
        bool heldClick = false;
        bool clicked = false;
        bool mouseDown = false;
        bool followMouse = false;

    public:
        // Constructor
        explicit Piece(char _colID);
        virtual ~Piece();

        // Setup
        void SetPos(std::pair<char, int> _position);

        /*
         * DISPLAY
         */

        // Creating textures
        int CreateTextures();
        void SetRects(const std::unique_ptr<Board>& _board);
        void GetRectOfBoardPosition(const std::unique_ptr<Board>& _board);

        // Displaying piece / moves
        void DisplayPiece(const std::unique_ptr<Board>& _board);
        void DisplayMoves(const std::unique_ptr<Board>& _board);

        /*
         * Fetching and testing Moves
         */

        // Fetching and testing Moves
        virtual void FetchMoves(const std::vector<std::unique_ptr<Piece>> &_teamPieces,
                                const std::vector<std::unique_ptr<Piece>> &_oppPieces,
                                const std::unique_ptr<Board>& _board);
        void EnforceBorderOnMoves(const std::unique_ptr<Board>& _board);
        bool MoveLeadsToCheck(const std::vector<std::unique_ptr<Piece>> &_teamPieces,
                                     const std::vector<std::unique_ptr<Piece>> &_oppPieces,
                                     const std::unique_ptr<Board>& _board,
                                     const AvailableMove& _move);
        void PreventMoveIntoCheck(const std::vector<std::unique_ptr<Piece>> &_teamPieces,
                                  const std::vector<std::unique_ptr<Piece>> &_oppPieces,
                                  const std::unique_ptr<Board>& _board);

        // Clear moves
        void ClearMoves();
        void ClearNextMoves();

        /*
         * TARGETING INFO / POSITIONAL INFO FETCHING
         */

        // Target / Targeting Status Fetching
        bool IsTargetingPosition(std::pair<char, int> _targetPosition);
        bool IsCheckingKing();

        // Fetching piece if on a particular position
        static Piece* GetTeamPieceOnPosition(const std::vector<std::unique_ptr<Piece>> &_teamPieces,
                                             std::pair<char, int> _targetPos);
        static Piece* GetOppPieceOnPosition(const std::vector<std::unique_ptr<Piece>> &_oppPieces,
                                            std::pair<char, int> _targetPos);
        static Piece* GetPieceOnPosition(const std::vector<std::unique_ptr<Piece>> &_teamPieces,
                                         const std::vector<std::unique_ptr<Piece>> &_oppPieces,
                                         std::pair<char, int> _targetPos);

        /*
         * MAKING A MOVE
         */

        // Making a move / Testing a move
        void MoveTo(std::pair<char, int> _movepos,
                    const std::unique_ptr<Board>& _board);
        virtual void UpdateCheckerVars();
        void Captured(bool captured = true);
        void TempMoveTo(const AvailableMove& _tempmove);
        void UnMove(const AvailableMove& _tempmove);

        // Promotions
        bool ReadyToPromote(const std::unique_ptr<Board>& _board);
        void UpdatePromoteInfo(const std::unique_ptr<Piece>& _promotedTo);
        [[nodiscard]] Piece* GetPromotedTo() const;

        /*
         * SELECTING PIECES
         */

        // Selecting Piece
        void UpdateClickedStatus(const std::vector<std::unique_ptr<Piece>> &_teamPieces);
        void SetSelected(bool _selected);
        void UnselectPiece();

        /*
         * GETTERS
         */

        PieceInfo* GetPieceInfoPtr() { return info.get(); };
        std::unique_ptr<std::vector<AvailableMove>> GetAvailableMovesPtr() {
            return std::make_unique<std::vector<AvailableMove>>(validMoves); };
        [[nodiscard]] bool HasMoved() const { return hasMoved; };
        [[nodiscard]] bool IsCaptured() const { return captured; };
        [[nodiscard]] bool IsClicked() const { return clicked; };
        [[nodiscard]] bool CanPassant() const { return canPassant; };
        [[nodiscard]] std::pair<char, int> GetPassantTarget() const {
            return {info->gamepos.first, info->gamepos.second - dir};
        };

        // should be moved to child
        std::pair<bool, bool> CanCastle();
};

/*
 * Local Enums
 */

enum Piece::RectID : int {
        PIECE_RECT, BOARDPOS_RECT,
};


#endif //CHESS_WITH_SDL_PIECE_H
