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
#include "Board.h"

class Piece;
class Board;

struct Piece_Info {
    std::string name = "Pawn";
    char pieceID = 'a';
    std::string color = "White";
    char colID = 'W';
    Position<char, int> gamepos {'A', 7};
};

struct AvailableMove{
    Position<char, int> position {};
    bool capture = false;
    Piece* target = nullptr;
};

class Piece {
    protected:
        // Game mechanics / position
        std::vector<AvailableMove> validMoves {};
        bool captured = false;
        bool updatedMoves = false;

        // Piece identification
        Piece_Info* info {};

        // SDL display
        std::string imgName {};
        SDL_Texture* pieceTexture {};
        SDL_Texture* moveHighlights[3] {};
        SDL_Rect pieceRect {};
        SDL_Rect boardposRect {};
        float b_width = 0.1;

        // pawn movements
        bool hasMoved = false;
        bool canPassant = false;
        bool canPromote = false;

        int dir = 1;

        // user input detection
        bool selected = false;
        bool followMouse = false;

    public:
        Piece(const std::string& _name, const std::string& _color, Position<char, int> _target);
        int CreateTextures();
        void GetRectOfBoardPosition(const Board& board);
        void DisplayPiece();

        // Fetching Moves
        static int PositionOccupied(const std::vector<Piece*> &_teamPieces, const std::vector<Piece*> &_oppPieces, Position<char, int> _targetPos);
        static Piece* GetOpponentOnPosition(const std::vector<Piece*> &_oppPieces, Position<char, int> _targetPos);
        void EnforceBorderOnMoves();
        virtual void FetchMoves(const std::vector<Piece*> &_teamPieces, const std::vector<Piece*> &_oppPieces, const Board& _board);
        void ClearMoves();
        virtual void UpdateCheckerVars();

        void Captured();
        void DisplayMoves(const Board& board);
        bool CheckClicked();
        bool UpdateSelected();
        void UnselectPiece();

        Piece_Info* GetPieceInfoPtr() { return info; };
        std::vector<AvailableMove>* GetAvailableMovesPtr() { return &validMoves; };
        void MoveTo(Position<char, int> _movepos, const Board& _board);
};


#endif //CHESS_WITH_SDL_PIECE_H
