//
// Created by cew05 on 21/06/2024.
//

#include "include/GameScreen.h"

GameScreen::GameScreen(char _teamID) : AppScreen() {
    // Temp vars
    Menu* menu;
    Button* button;
    GenericManager<Button*>* menuButtonManager;

    // Setup for pieces and board
    SetUpBoard();
    SetUpPieces();

    board->WriteStartPositionsToFile(*allPieces);

    // Set users team pointer
    if (_teamID == 'W') {
        userTeamPtr = whitePieces;
        usersTurn = true;
    }
    else {
        userTeamPtr = blackPieces;
        usersTurn = false;
    }

    teamptr = whitePieces;
    oppptr = blackPieces;

    /*
     * Construct OPTIONS menu (back to menu, resign, offer draw)
     */

    // button to show menu
//    button = new Button({0, 0}, {30, 30}, "HI");
//    buttonManager->NewResource(button, SHOW_OPTIONS_MENU);

    menu = new Menu({0, 0}, {100, 500}, "");
    menu->CanClose(false);
    menuManager->NewResource(menu, OPTIONS_MENU);

    // Set states

    stateManager->NewResource(false, SHOW_PROMO_MENU);
    stateManager->NewResource(false, END_OF_TURN);
    stateManager->NewResource(true, ALL_TASKS_COMPLETE);
    stateManager->NewResource(false, CHECKMATE);
    stateManager->NewResource(false, STALEMATE);
}

GameScreen::~GameScreen() {
    // Ensure stockfish process is closed
    if (sfm != nullptr) {
        delete sfm;
        sfm = nullptr;
    }

}

void GameScreen::SetUpBoard() {
    // Construct board
    board = new Board();
    board->SetBoardPos(100, 0);

    // Ensure GameData directory exists else end program
    if (!board->GameDataDirectoryExists()) {
        printf("Failed to ensure game directory exists");
        return;
    }
    board->ClearExcessGameFiles();
    // Create game data files in dir
    if (!board->CreateGameFiles()) {
        printf("Error creating game files.\n");
        return;
    }


}

void GameScreen::SetUpPieces() {
    // Read standard board from file
    std::fstream boardStandardFile("../RequiredFiles/BasicSetup.csv");
    std::string pieceString;

    std::vector<std::string> pieceElements;
    char div = ',';
    uint64_t splitpos;

    while (std::getline(boardStandardFile, pieceString)) {
        pieceElements.clear();

        while ((splitpos = pieceString.find(div)) != std::string::npos) {
            pieceElements.push_back(pieceString.substr(0, splitpos));
            pieceString.erase(0, splitpos + 1);
        }
        pieceElements.push_back(pieceString);

        // construct Piece from elements taken from file
        Piece* newPiece = nullptr;
        if (pieceElements[1] == "Pawn") {
            newPiece = new Piece("Pawn", 'W', {});
            newPiece = new Piece("Pawn", pieceElements[0][0], {(char)pieceElements[2][0], std::stoi(pieceElements[3])});
        }
        if (pieceElements[1] == "Knight") {
            newPiece = new Knight("Knight", pieceElements[0][0], {(char)pieceElements[2][0], std::stoi(pieceElements[3])});
        }
        if (pieceElements[1] == "Bishop") {
            newPiece = new Bishop("Bishop", pieceElements[0][0], {(char)pieceElements[2][0], std::stoi(pieceElements[3])});
        }
        if (pieceElements[1] == "Rook") {
            newPiece = new Rook("Rook", pieceElements[0][0], {(char)pieceElements[2][0], std::stoi(pieceElements[3])});
        }
        if (pieceElements[1] == "King") {
            newPiece = new King("King", pieceElements[0][0], {(char)pieceElements[2][0], std::stoi(pieceElements[3])});
        }
        if (pieceElements[1] == "Queen") {
            newPiece = new Queen("Queen", pieceElements[0][0], {(char)pieceElements[2][0], std::stoi(pieceElements[3])});
        }

        if (newPiece != nullptr) {
            newPiece->CreateTextures();
            newPiece->GetRectOfBoardPosition(board);
            (pieceElements[0][0] == 'W') ? whitePieces->push_back(newPiece) : blackPieces->push_back(newPiece);
            allPieces->push_back(newPiece);
        }
    }
    boardStandardFile.close();

    for (auto& piece : *allPieces) {
        piece->SetRects(board);
    }

    printf("CONSTRUCTED %zu WHITE PIECES, %zu BLACK PIECES, %zu TOTAL PIECES\n",
           whitePieces->size(), blackPieces->size(), allPieces->size());
}

void GameScreen::SetupEngine(bool _limitStrength, int _elo, int _level) {
    std::string funcStr;

    // Setup stockfish
    if (sfm == nullptr) {
        printf("WARNING SUBPROCESS STOCKFISH OPENED, CHECK FOR CLOSURE ON PROGRAM END\n");
        sfm = new StockfishManager();
    }
    else {
        // SF already opened, indicate a new game by setting the start position
        funcStr = "position startpos\n";
        sfm->DoFunction(funcStr);
    }

    // pass commands to set engine difficulty
    funcStr = "setoption name UCI_LimitStrength value ";
    funcStr += ((_limitStrength) ? "true\n" : "false\n");
    sfm->DoFunction(funcStr);

    if (_limitStrength) {
        // ensure min/max elo bounds not exceeded
        _elo = std::min(_elo, 3190);
        _elo = std::max(_elo, 1320);
        funcStr = "setoption name UCI_Elo value " + std::to_string(_elo) + "\n";
        sfm->DoFunction(funcStr);

        // ensure min/max skill level bounds not exceeded
        _level = std::min(_level, 20);
        _level = std::max(_level, 0);
        funcStr = "setoption name Skill Level value " + std::to_string(_level) + "\n";
        sfm->DoFunction(funcStr);
    }

    funcStr = "setoption name UCI_LimitStrength value ";
    funcStr += ((_limitStrength) ? "true\n" : "false\n");
    sfm->DoFunction(funcStr);
}

std::string GameScreen::FetchOpponentMove() {
    // Get FEN of current position
    std::string FENstr = board->CreateFEN(*whitePieces, *blackPieces);
    printf("%s\n", FENstr.c_str());

    return FENstr;
}

std::string GameScreen::FetchOpponentMoveEngine() {
    // Get FEN of current position
    std::string FENstr = board->CreateFEN(*whitePieces, *blackPieces);
    printf("GET MOVE FROM FEN %s\n", FENstr.c_str());

    if (sfm == nullptr) {
        SetupEngine(true, 1500, 10);
    }

    std::string cmd;

    // Get response to request to find bestmove
    cmd = "position fen " + FENstr + "\n";
    sfm->DoFunction(cmd);

    sfm->DoFunction("go depth 10\n");

    // read lines of response untill finding "bestmove ..." line
    std::string response = sfm->FetchResult(), line;
    char delim = '\n';
    size_t pos = response.find(delim);

    while ((line = response.substr(0, pos+1)).find("bestmove") == std::string::npos) {
        //printf("Line %s\n", line.c_str());
        response.erase(0, pos+1);
        if (response.empty()) response = sfm->FetchResult();
        pos = response.find(delim);
    }
    //printf("Success Line %s\n", line.c_str());

    // convert response string into only movestring [targetpos][destpos][promoteTo]
    delim = ' ';
    pos = line.find(delim);
    line.erase(0, pos+1);
    //printf("DEBUG REMOVE BESTMOVE [%s]\n", line.c_str());

    if ((pos = line.find(delim)) != std::string::npos) {
        line.erase(pos, std::string::npos);
    }
    //printf("DEBUG REMOVE BESTMOVE [%s]\n", line.c_str());

    return line;
}

bool GameScreen::CreateTextures() {
    if (!AppScreen::CreateTextures()) return false;

    // Create board textures
    board->CreateBoardTexture();
    board->CreatePromoMenuTexture();

    // Create piece textures
    for (auto& piece : *allPieces) {
        //piece->CreateTextures();
        piece->SetRects(board);
    }

    return true;
}

bool GameScreen::Display() {
    bool state;
    Menu* menu;
    Button* button;

    // Display screen background
    SDL_SetRenderDrawColor(window.renderer, 0, 150, 150, 255);
    SDL_RenderFillRect(window.renderer, &window.currentRect);
    SDL_SetRenderDrawColor(window.renderer, 0, 0, 0, 0);

    // Display board
    board->DisplayGameBoard();
    stateManager->FetchResource(state, SHOW_PROMO_MENU);

    // Display buttons
    for (auto& bmButton : *buttonManager->AccessMap()) {
        bmButton.second->Display();
    }


    // Display Pieces
    for (Piece* piece : *allPieces) {
        piece->DisplayPiece(board);
        piece->DisplayMoves(board);
    }

    // Display the promotion menu if required
    if (state) board->DisplayPromoMenu(teamptr->back());

    // If end of game, display the endgame menu


    // if show menu clicked show options menu
    menuManager->FetchResource(menu, OPTIONS_MENU);
    menu->Display();

//    for (auto& bmMenu : *menuManager->AccessMap()) {
//        bmMenu.second->Display();
//    }

    return true;
}

void GameScreen::ResizeScreen() {
    AppScreen::ResizeScreen();

    // Temp vars
    Menu* menu;
    Button* button;

    // Resize board
    board->FillToBounds(window.currentRect.w, window.currentRect.h);

    // Resize pieces
    // ...

    // Resize options menu
    menuManager->FetchResource(menu, OPTIONS_MENU);
    menu->UpdateSize({100, window.currentRect.h});
    menuManager->ChangeResource(menu, OPTIONS_MENU);
}

void GameScreen::HandleEvents() {
    AppScreen::HandleEvents();

    // game states
    bool eot;
    bool allTasksComplete;

    stateManager->FetchResource(eot, END_OF_TURN);
    stateManager->FetchResource(allTasksComplete, ALL_TASKS_COMPLETE);

    /*
     * FETCH PIECE MOVES
     */

    for (Piece* piece : *teamptr) {
        piece->ClearMoves();
        piece->ClearNextMoves();
        piece->FetchMoves(*teamptr, *oppptr, *board);
        piece->PreventMoveIntoCheck(*teamptr, *oppptr, *board);
    }

    /*
     * CHECK FOR STALEMATE / CHECKMATE
     * TODO : 3 move repetition
     * TODO : 50 moves rule
     * TODO : insufficient material
     */

    bool canMove = std::any_of(teamptr->begin(), teamptr->end(), [](Piece *piece) {
        return !piece->GetAvailableMovesPtr()->empty();
    });

    if (!canMove) {
        // if there are no moves available, check if King is being checked by opp
        if (std::any_of(oppptr->begin(), oppptr->end(), [](Piece *piece) {
            return piece->IsCheckingKing();
        })) {
            // conditions met: checkmate
            printf("CHECKMATE! 1:0");
            stateManager->ChangeResource(true, CHECKMATE);
        } else {
            // conditions met: stalemate
            printf("STALEMATE! 0.5:0.5");
            stateManager->ChangeResource(true, STALEMATE);
        }

        // Show results, store final game results, wait for input to return to menu
        return;
    }

    /*
     * HANDLE USER INPUT FOR MOVES
     */

    if (usersTurn) {
        for (Piece* piece : *userTeamPtr) {
            piece->UpdateClickedStatus(userTeamPtr);
        }

        // check if user has clicked on a move
        if (selectedPiece->CheckForMoveClicked(board)) {
            // make move
            selectedPiece->MakeMove(board);
            eot = true;
        }

        // check if user clicks on a piece
        if (!eot) {
            selectedPiece->CheckForPieceClicked(userTeamPtr);
        }
    }

    /*
     * HANDLE OPPONENT INPUT FOR MOVES
     */

    if (!usersTurn) {
        // [position of piece][destination position][promotion] needs to be converted into an actual move
        std::string basicMoveStr = FetchOpponentMoveEngine();
        printf("movegiven : %s\n", basicMoveStr.c_str());

        std::pair<char, int> pos = {basicMoveStr[0], basicMoveStr[1] - '0'};
        std::pair<char, int> target = {basicMoveStr[2], basicMoveStr[3] - '0'};
        Piece* movPiece = Piece::GetTeamPieceOnPosition(*teamptr, pos);
        if (movPiece == nullptr) {
            // Big issue!!!!
            printf("failed to find moving piece at %c%d. CHECK FEN STRING\n", pos.first, pos.second);
        }
        else {
            for (auto move : *movPiece->GetAvailableMovesPtr()) {
                if (target.first == move.GetPosition().first && target.second == move.GetPosition().second) {
                    selectedPiece->MakeMove(movPiece, &move, board);
                    break;
                }
            }

            // Promotion
            if (basicMoveStr.length() > 4) {
                Piece* promotedPiece = nullptr;
                Piece_Info* pi = movPiece->GetPieceInfoPtr();
                switch (basicMoveStr[4]) {
                    case 'n':
                        promotedPiece = new Knight("Knight", pi->colID, pi->gamepos);
                        break;
                    case 'b':
                        promotedPiece = new Bishop("Bishop", pi->colID, pi->gamepos);
                        break;
                    case 'r':
                        promotedPiece = new Rook("Rook", pi->colID, pi->gamepos);
                        break;
                    case 'q':
                        promotedPiece = new Queen("Queen", pi->colID, pi->gamepos);
                        break;
                    default: break;
                }

                if (promotedPiece != nullptr) {
                    printf("Settting piece\n");
                    // Piece has been made, mark pawn as captured and add new piece to teamptr
                    movPiece->Captured(true);
                    movPiece->UpdatePromoteInfo(promotedPiece);

                    promotedPiece->CreateTextures();
                    promotedPiece->SetRects(board);

                    teamptr->push_back(promotedPiece);
                    allPieces->push_back(promotedPiece);
                }
            }
        }


        eot = true;
    }


    /*
     * PROMOTIONS
     */

    if (eot && usersTurn) {
        for (auto piece : *userTeamPtr) {
            if (piece->ReadyToPromote(*board)) {
                allTasksComplete = false;

                // show promo menu
                stateManager->ChangeResource(true, SHOW_PROMO_MENU);

                Piece* promotedPiece = nullptr;
                std::string promoteTo = board->GetPromoMenuInput();
                Piece_Info* pi = piece->GetPieceInfoPtr();

                if (promoteTo == "Knight") {
                    promotedPiece = new Knight("Knight", pi->colID, pi->gamepos);
                }
                else if (promoteTo == "Bishop") {
                    promotedPiece = new Bishop("Bishop", pi->colID, pi->gamepos);
                }
                else if (promoteTo == "Rook") {
                    promotedPiece = new Rook("Rook", pi->colID, pi->gamepos);
                }
                else if (promoteTo == "Queen"){
                    promotedPiece = new Queen("Queen", pi->colID, pi->gamepos);
                }

                if (promotedPiece != nullptr) {
                    // Piece has been made, mark pawn as captured and add new piece to teamptr
                    piece->Captured(true);
                    piece->UpdatePromoteInfo(promotedPiece);

                    promotedPiece->CreateTextures();
                    promotedPiece->SetRects(board);

                    teamptr->push_back(promotedPiece);
                    allPieces->push_back(promotedPiece);

                    promotedPiece->FetchMoves(*teamptr, *oppptr, *board);

                    allTasksComplete = true;
                    stateManager->ChangeResource(false, SHOW_PROMO_MENU);
                }
            }
        }
    }

    /*
     * HANDLE END OF TURN
     */

    if (eot && allTasksComplete) {
        // update checker vars
        for (auto& piece : *allPieces) {
            piece->UpdateCheckerVars();
        }

        // Create and get the lastMove string
        selectedPiece->CreateACNstring(teamptr);
        board->WriteMoveToFile(selectedPiece->GetACNMoveString());

        std::swap(teamptr, oppptr);
        board->IncrementTurn();
        eot = false;

        // change turn
        usersTurn = !usersTurn;
    }

    // Update states
    stateManager->ChangeResource(eot, END_OF_TURN);
    stateManager->ChangeResource(allTasksComplete, ALL_TASKS_COMPLETE);
}

void GameScreen::CheckButtons() {
    AppScreen::CheckButtons();
}