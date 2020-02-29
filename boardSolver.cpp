#include "game_logic.h"

gameSolver::gameSolver(gameBoard& initBoard):
	currBoard(initBoard){
	remMines = initBoard.getMines();
}

void gameSolver::solveTile(int r, int c) {
	if (solveQueue.empty()) {
		return;
	}
	else {
		// The following code block is the logic to solve a tile on minesweeper.
	}
}

void gameSolver::fillReset() {
	while (!resetQueue.empty()) {
		resetQueue.pop();
	}
	gameTile* tileIterator;
	for (int i = 1; i <= currBoard.getHeight(); i++) {
		for (int j = 1; j <= currBoard.getWidth(); j++) {
			tileIterator = currBoard(i, j);
			if (tileIterator->getState() == 'E') {
				resetQueue.push(tileIterator);
				possMineLocs.push_back(tileIterator);
			}
		}
	}
}

void gameSolver::resetRemainder() {
	int height = currBoard.getHeight();
	int width = currBoard.getWidth();
	gameTile* tileIterator;
	int minesRemoved;
	int vPotMineIndex;
	gameTile* mineTile;
	while (!resetQueue.empty()) {
		tileIterator = resetQueue.front();
		resetQueue.pop();
		if (tileIterator->isMine()) {
			tileIterator->removeMine();
			minesRemoved++;
		}
		while (minesRemoved != 0) {
			vPotMineIndex = rand() % (height * width);
			int iH = (vPotMineIndex / height) + 1;
			int iW = (vPotMineIndex % width) + 1;
			mineTile = (currBoard(iH,iW));
			if (mineTile->isMine() == 0) {
				mineTile->setMine();
				currBoard.doOnAllAdj(&gameTile::incAdjMines, iH, iW);
				minesRemoved--;
			}
		}
	}
}

void gameSolver::solveBoard() {
	return;
}

void gameSolver::solverFlag(int r, int c) {

}

void gameSolver::solverPress(int r, int c) {
	currBoard.pressTile(r, c);
	addAdjToSQueue(r, c);
}

void gameSolver::fillSQueue(int r, int c) {
	int height = currBoard.getHeight() + 1;
	int width = currBoard.getWidth() + 1;
	for (int i = 0; i <= height; i++) {
		for (int j = 0; j <= width; j++) {
			if (currBoard(i, j)->getState() == 'P') {
				addAdjToSQueue(i, j);
			}
		}
	}

}

void gameSolver::addAdjToSQueue(int r, int c) {
	int height = currBoard.getHeight() + 1;
	int width = currBoard.getWidth() + 1;
	for (signed int i = -1; i <= 1; i++) {
		for (signed int j = -1; j <= 1; j++) {
			if (i == 0 && j == 0) {
				continue;
			}
			if (r + i < 0 || r + i > height || c + j < 0 || c + j > width) {
				continue;
			}
			if (currBoard(i, j)->getState() == 'E') {
				solveQueue.push(currBoard(r + i, c + j));
			}
		}
	}
}

/*
ofstream initBoard;
initBoard.open("mineBoard.txt", ios::binary);
if (initBoard.is_open()) {
	printCheat(initBoard);
}
if (initBoard.is_open()) {
	printBoard(initBoard);
	initBoard.close();
}

*/