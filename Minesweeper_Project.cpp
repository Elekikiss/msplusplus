#include <cstdlib>
#include <iostream>
#include <fstream>
#include "game_logic.h"

/* This minesweeper module is intended to eventually be ported to a GUI;
** however, for the sake of the project, all of the testing will be done within
** a console.*/

int main() {
    ifstream initOptions;
    ofstream userOptions;
    boardOptions* currBoardOps;
    char userChoice;

    std::cout << "Debug Statement: opening settings file.\n";
    initOptions.open("minesweeper.bin", ios::binary);
    if (initOptions.is_open()) {
        std::cout << "Debug Statement: opened settings file.\n";
        currBoardOps = new boardOptions;
        int w;
        int h;
        int m;
        initOptions >> w;
        initOptions >> h;
        initOptions >> m;
        currBoardOps->setOpt(w, h, m);
        // Add initialization of Statistics data later, if time permits
        initOptions.close();
        std::cout << "Debug Statement: read and closed settings file.\n";
    }
    else {
        std::cout << "Debug Statement: settings not found\n";
        currBoardOps = new boardOptions;
    }
    while (true) {
        std::cout << "Change settings? (Y/N)\n";
        std::cin >> userChoice;
        userChoice = toupper(userChoice);
        if (userChoice != 'Y' && userChoice != 'N') {
            std::cout << "Invalid Response, try again.\n";
        }
        else {
            break;
        }
    }
    if (userChoice == 'Y') {
        while (true) {
            std::cout << "(H)ard, (M)edium, (E)asy, or (C)ustom?\n";
            std::cin >> userChoice;
            userChoice = toupper(userChoice);
            if (userChoice != 'H' && userChoice != 'M' && userChoice != 'E' && userChoice != 'C') {
                std::cout << "Invalid Response, try again.\n";
            }
            else {
                break;
            }
        }
        switch (userChoice) {
        case 'H':
            std::cout << "Setting: Hard\n";
            currBoardOps->setOpt(MINESWEEPER_HARD_WIDTH, MINESWEEPER_HARD_HEIGHT, MINESWEEPER_HARD_MINES);
            break;
        case 'M':
            std::cout << "Setting: Medium\n";
            currBoardOps->setOpt(MINESWEEPER_MEDIUM_WIDTH, MINESWEEPER_MEDIUM_HEIGHT, MINESWEEPER_MEDIUM_MINES);
            break;
        case 'E':
            std::cout << "Setting: Easy\n";
            currBoardOps->setOpt(MINESWEEPER_EASY_WIDTH, MINESWEEPER_EASY_HEIGHT, MINESWEEPER_EASY_MINES);
            break;
        case 'C':
            std::cout << "Setting: Custom\n";
            while (true) {
                int w;
                int h;
                int m;
                std::cout << "Enter desired width: ";
                std::cin >> w;
                std::cout << "Enter desired height: ";
                std::cin >> h;
                std::cout << "Enter desired mine count: ";
                std::cin >> m;
                if (currBoardOps->setOpt(w, h, m)) {
                    break;
                }
                else {
                    std::cout << "Invalid mine count, try again.\n";
                }
            }
        }
    }

    gameBoard* currBoard;
    currBoard = new gameBoard(*currBoardOps, 123456789);
    currBoard->printBoard(std::cout);
    std::cout << "\n\n\n";
    delete currBoard;
    userOptions.open("minesweeper.bin", ios::binary);
    if (userOptions.is_open()) {
        userOptions << currBoardOps->getWidth() << "\n"
            << currBoardOps->getHeight() << "\n"
            << currBoardOps->getMines() << "\n";
        // Add depositing of Statistics data later, if time permits
        userOptions.close();
    }
    delete currBoardOps;
    return 0;
}
