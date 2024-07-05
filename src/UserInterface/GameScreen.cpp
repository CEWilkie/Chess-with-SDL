//
// Created by cew05 on 21/06/2024.
//

#include <memory>

#include "include/GameScreen.h"

GameScreen::GameScreen(char _teamID) : AppScreen() {
    // Temp vars
    Menu* menu;
    Button* button;
    GenericManager<Button*>* menuButtonManager;

    // Setup for pieces and board
    teamPieces = std::make_unique<std::vector<std::unique_ptr<Piece>>>();  // set team as white goes first (hence is current team)
    oppPieces = std::make_unique<std::vector<std::unique_ptr<Piece>>>();  // set opp as black

    SetUpBoard();
    SetUpPieces();

    board->WriteStartPositionsToFile(*teamPieces, *oppPieces); // whitePieces , blackPieces

    // Set users turn
    usersTurn = (_teamID == 'W');

    /*
     * Construct OPTIONS menu (back to menu, resign, offer draw)
     */

    menu = new Menu({0, 0}, {100, 500}, "");
    menu->CanClose(false);

    // Back to home screen
    button = new Button({10, 10}, {80, 30}, "Home");
    menu->AccessButtonManager()->NewResource(button, OM_HOME_SCREEN);

    // Resign game
    button = new Button({10, 110}, {80, 30}, ICON_RESIGN);
    menu->AccessButtonManager()->NewResource(button, OM_RESIGN);

    // Offer Draw
    button = new Button({10, 210}, {80, 30}, ICON_OFFERDRAW);
    menu->AccessButtonManager()->NewResource(button, OM_OFFER_DRAW);

    // New Game
    button = new Button({10, 310}, {80, 30}, "New Game");
    menu->AccessButtonManager()->NewResource(button, OM_NEWGAME);

    // Flip Board
    button = new Button({10, 410}, {80, 30}, "Flip Board");
    menu->AccessButtonManager()->NewResource(button, OM_FLIP_BOARD);

    menuManager->NewResource(menu, OPTIONS_MENU);

    // Set states

    stateManager->NewResource(false, SHOW_PROMO_MENU);
    stateManager->NewResource(false, END_OF_TURN);
    stateManager->NewResource(true, ALL_TASKS_COMPLETE);
    stateManager->NewResource(false, CHECKMATE);
    stateManager->NewResource(false, STALEMATE);
}

void GameScreen::SetUpBoard() {
    // Setup Board
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
    // Clear old pieces
    teamPieces->clear();
    oppPieces->clear();

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
        std::unique_ptr<Piece> newPiecePtr = nullptr;

        if (pieceElements[1] == "Pawn") {
            newPiecePtr = std::make_unique<Piece>(pieceElements[0][0]);
        }
        if (pieceElements[1] == "Knight") {
            newPiecePtr = std::make_unique<Knight>(pieceElements[0][0]);
        }
        if (pieceElements[1] == "Bishop") {
            newPiecePtr = std::make_unique<Bishop>(pieceElements[0][0]);
        }
        if (pieceElements[1] == "Rook") {
            newPiecePtr = std::make_unique<Rook>(pieceElements[0][0]);
        }
        if (pieceElements[1] == "King") {
            newPiecePtr = std::make_unique<King>(pieceElements[0][0]);
        }
        if (pieceElements[1] == "Queen") {
            newPiecePtr = std::make_unique<Queen>(pieceElements[0][0]);
        }

        if (newPiecePtr != nullptr) {
            newPiecePtr->CreateTextures();
            newPiecePtr->SetPos({(char)pieceElements[2][0], std::stoi(pieceElements[3])});
            newPiecePtr->SetRects(board);

            if (pieceElements[0][0] == 'W') teamPieces->push_back(std::move(newPiecePtr));
            else oppPieces->push_back(std::move(newPiecePtr));
        }
    }
    boardStandardFile.close();

    printf("CONSTRUCTED %zu WHITE PIECES, %zu BLACK PIECES, %zu TOTAL PIECES\n",
           teamPieces->size(), oppPieces->size(), teamPieces->size() + oppPieces->size());
}

void GameScreen::SetupEngine(bool _limitStrength, int _elo, int _level) {
    std::string funcStr;

    // Setup stockfish
    if (sfm == nullptr) {
        printf("WARNING SUBPROCESS STOCKFISH OPENED, CHECK FOR CLOSURE ON PROGRAM END\n");
        sfm = std::make_unique<StockfishManager>();
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
    std::string FENstr = board->CreateFEN(*teamPieces, *oppPieces);
    printf("%s\n", FENstr.c_str());

    return FENstr;
}

std::string GameScreen::FetchOpponentMoveEngine(const std::vector<std::unique_ptr<Piece>>& _teamPieces,
                                                const std::vector<std::unique_ptr<Piece>>& _oppPieces) {
    // Get FEN of current position
    std::string FENstr = board->CreateFEN(_teamPieces, _oppPieces);
    //printf("GET MOVE FROM FEN %s\n", FENstr.c_str());

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

    // remove trailing text after space
    if ((pos = line.find(delim)) != std::string::npos) {
        line.erase(pos, std::string::npos);
    }

    // if the line length remains over 4, check for promotion char else remove chars
    if (line.length() > 4) {
        if (!isalpha(line[4])) line.erase(4, std::string::npos);
    }

    return line;
}

bool GameScreen::CreateTextures() {
    if (!AppScreen::CreateTextures()) return false;

    // Create board textures
    board->CreateBoardTexture();
    board->CreatePromoMenuTexture();

    // Create team piece textures
    for (const auto& piece : *teamPieces) {
        //piece->CreateTextures();
        piece->SetRects(board);
    }

    // Create opp piece textures
    for (const auto& piece : *oppPieces) {
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
    SDL_SetRenderDrawColor(window.renderer, 102, 57, 49, 255);
    SDL_RenderFillRect(window.renderer, &window.currentRect);
    SDL_SetRenderDrawColor(window.renderer, 0, 0, 0, 0);

    // Display board
    board->DisplayGameBoard();

    // Display team Pieces
    for (const auto& piece : *teamPieces) {
        piece->DisplayPiece(board);
        piece->DisplayMoves(board);
    }

    // Display opp Pieces
    for (const auto& piece : *oppPieces) {
        piece->DisplayPiece(board);
        piece->DisplayMoves(board);
    }

    // Display the promotion menu if required
    stateManager->FetchResource(state, SHOW_PROMO_MENU);
    if (state) {
        board->DisplayPromoMenu(teamPieces->back());
    }

    // Display options menu
    menuManager->FetchResource(menu, OPTIONS_MENU);
    menu->Display();

    // If end of game, display the endgame menu
    // ...

    return true;
}

void GameScreen::ResizeScreen() {
    AppScreen::ResizeScreen();

    // Temp vars
    SDL_Rect objRect;
    Menu* menu;
    Button* button;

    // Resize options menu to fit max dimensions
    menuManager->FetchResource(menu, OPTIONS_MENU);
    if (menu->FetchMenuRect().w > 100) menu->UpdateSize({150, menu->FetchMenuRect().h});
    menuManager->ChangeResource(menu, OPTIONS_MENU);

    // Resize board
    board->FillToBounds(window.currentRect.w, window.currentRect.h);
    menuManager->FetchResource(menu, OPTIONS_MENU);
    objRect = menu->FetchMenuRect();
    board->SetBoardPos(objRect.w, 0);

    // Resize pieces
    // ...
}

void GameScreen::HandleEvents() {
    AppScreen::HandleEvents();

    // If end of game has been reached, do not proceed with event loop
    bool cm = false, sm = false;
    stateManager->FetchResource(cm, CHECKMATE);
    stateManager->FetchResource(sm, STALEMATE);
    if (cm || sm) return;

    // game states
    bool eot;
    bool allTasksComplete;

    stateManager->FetchResource(eot, END_OF_TURN);
    stateManager->FetchResource(allTasksComplete, ALL_TASKS_COMPLETE);

    /*
     * FETCH PIECE MOVES
     */

    for (const auto& piece : *teamPieces) {
        piece->ClearMoves();
        piece->ClearNextMoves();
        piece->FetchMoves(*teamPieces, *oppPieces, board);
        piece->PreventMoveIntoCheck(*teamPieces, *oppPieces, board);
    }

    /*
     * CHECK FOR STALEMATE / CHECKMATE
     * TODO : 3 move repetition
     * TODO : 50 moves rule
     * TODO : insufficient material
     */

    bool canMove = std::any_of(teamPieces->begin(), teamPieces->end(), [](const std::unique_ptr<Piece>& piece) {
        return !piece->GetAvailableMovesPtr()->empty();
    });

    if (!canMove) {
        // if there are no moves available, check if King is being checked by opp
        if (std::any_of(oppPieces->begin(), oppPieces->end(), [](const std::unique_ptr<Piece>& piece) {
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
    }

    /*
     * HANDLE USER INPUT FOR MOVES
     */

    if (usersTurn) {
        for (const auto& piece : *teamPieces) {
            piece->UpdateClickedStatus(*teamPieces);
        }

        // check if user has clicked on a move
        if (selectedPiece->CheckForMoveClicked(board)) {
            // make move
            selectedPiece->MakeMove(board);

            eot = true;
        }

        // check if user clicks on a piece
        if (!eot) {
            selectedPiece->CheckForPieceClicked(*teamPieces);
        }
    }

    /*
     * HANDLE OPPONENT INPUT FOR MOVES (ENGINE)
     */

    // [position of piece][destination position][promotion] needs to be converted into an actual move
    std::string basicMoveStr;

    if (!usersTurn) {
        basicMoveStr = FetchOpponentMoveEngine(*teamPieces, *oppPieces);
        //printf("movegiven : %s, L:%zu\n", basicMoveStr.c_str(), basicMoveStr.length());

        std::pair<char, int> pos = {basicMoveStr[0], basicMoveStr[1] - '0'};
        std::pair<char, int> target = {basicMoveStr[2], basicMoveStr[3] - '0'};
        Piece* movPiece = Piece::GetTeamPieceOnPosition(*teamPieces, pos);
        if (movPiece == nullptr) {
            // Big issue!!!!
            printf("failed to find moving piece at %c%d. CHECK FEN STRING\n", pos.first, pos.second);
        }
        else {
            auto moves = *movPiece->GetAvailableMovesPtr();
            for (const auto& move : moves) {
                if (target.first == move.GetPosition().first && target.second == move.GetPosition().second) {
                    selectedPiece->MakeMove(movPiece, move, board);
                    break;
                }
            }
        }

        eot = true;
    }

    /*
     * HANDLE INPUT FOR OPPONENTS MOVE (NETWORK)
     */

    // ...

    /*
     * PROMOTIONS
     */

    if (eot) {
        std::unique_ptr<Piece> newPiecePtr = nullptr;
        Piece* promotingPiece = nullptr;
        PieceInfo* pi = nullptr;

        char promoteTo;

        // Get promoting piece + info
        for (const auto& piece : *teamPieces) {
            if (!piece->ReadyToPromote(board)) continue;

            // get promoting piece
            promotingPiece = piece.get();
            break;
        }

        if (promotingPiece != nullptr) {
            pi = promotingPiece->GetPieceInfoPtr();

            // Get input for promotion
            if (!usersTurn) promoteTo = basicMoveStr[4];
            else {
                stateManager->ChangeResource(true, SHOW_PROMO_MENU);
                promoteTo = board->GetPromoMenuInput();
            }

            switch (promoteTo) {
                case 'n':
                    newPiecePtr = std::make_unique<Knight>(pi->colID);
                    break;
                case 'b':
                    newPiecePtr = std::make_unique<Bishop>(pi->colID);
                    break;
                case 'r':
                    newPiecePtr = std::make_unique<Rook>(pi->colID);
                    break;
                case 'q':
                    newPiecePtr = std::make_unique<Queen>(pi->colID);
                    break;
                default:
                    break;
            }

            // Check if a new piece has been made
            if (newPiecePtr != nullptr) {
                // Piece has been made, mark pawn as captured and add new piece to whitePieces
                promotingPiece->Captured(true);
                promotingPiece->UpdatePromoteInfo(newPiecePtr);

                newPiecePtr->CreateTextures();
                newPiecePtr->SetPos(pi->gamepos);
                newPiecePtr->GetRectOfBoardPosition(board);
                newPiecePtr->SetRects(board);

                teamPieces->push_back(std::move(newPiecePtr));

                allTasksComplete = true;
                stateManager->ChangeResource(false, SHOW_PROMO_MENU);
            }
        }
    }

    /*
     * HANDLE END OF TURN
     */

    if (eot && allTasksComplete) {
        // update team checker vars
        for (const auto& piece : *teamPieces) {
            piece->UpdateCheckerVars();
        }

        // update opponent team checker vars
        for (const auto& piece : *oppPieces) {
            piece->UpdateCheckerVars();
        }

        // Create and get the lastMove string
        selectedPiece->CreateACNstring(*teamPieces);
        board->WriteMoveToFile(selectedPiece->GetACNMoveString());

        // change turn
        std::swap(teamPieces, oppPieces);
        board->IncrementTurn();
        usersTurn = !usersTurn;
        eot = false;
        printf("ENDTURN\n");

    }

    // Update states
    stateManager->ChangeResource(eot, END_OF_TURN);
    stateManager->ChangeResource(allTasksComplete, ALL_TASKS_COMPLETE);
}

void GameScreen::CheckButtons() {
    AppScreen::CheckButtons();

    // temp vars
    Menu* menu;
    Button* button;
    GenericManager<Button*>* buttonManager;

    /*
     * OPTIONSMENU
     */

    menuManager->FetchResource(menu, OPTIONS_MENU);
    buttonManager = menu->AccessButtonManager();

    // Return to homescreen
    buttonManager->FetchResource(button, OM_HOME_SCREEN);
    if (button->IsClicked()) {
        screenManager->FetchResource(currentScreen, HOMESCREEN);
        currentScreen->ResizeScreen();
        currentScreen->CreateTextures();
    }

    // Resign current game
    buttonManager->FetchResource(button, OM_RESIGN);
    if (button->IsClicked()) {
        stateManager->ChangeResource(true, RESIGN);
    }


    // Offer draw
    buttonManager->FetchResource(button, OM_OFFER_DRAW);
    if (button->IsClicked()) {
        stateManager->ChangeResource(true, DRAW_OFFER);
    }


    // New Game
    buttonManager->FetchResource(button, OM_NEWGAME);
    if (button->IsClicked()) {
        SetUpPieces();

        // Reset CM/SM
        stateManager->ChangeResource(false, CHECKMATE);
        stateManager->ChangeResource(false, STALEMATE);
    }


    // Flip board
    buttonManager->FetchResource(button, OM_FLIP_BOARD);
    if (button->IsClicked()) {
        stateManager->ChangeResource(true, BOARD_FLIPPED);
    }

}