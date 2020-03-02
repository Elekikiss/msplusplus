#include "game_logic.h"

gameSolver::gameSolver(gameBoard* initBoard){
	currBoard = initBoard;
	remMines = initBoard->getMines();
}

void gameSolver::solveBoard() {
	if(solveQueue.empty()) {
		return;
	}
	else {
		solveTile(solveQueue.front());
		solveQueue.pop();
	}
}

void gameSolver::solveTile(solverTile& target) {
	/* Minesweeper Solution Logic Type 1 2 1 */
	
	/* Comment Block Closer*/
	
	/* Minesweeper Solution Logic Type XXX */
	
	/* Comment Block Closer*/
	
	/* Minesweeper Solution Logic Type XXX */
	
	/* Comment Block Closer*/
	
	/* Minesweeper Solution Logic Type XXX */
	
	/* Comment Block Closer*/
}

void gameSolver::fillReset() {
	solverTile pushTile;
	while (!resetQueue.empty()) {
		resetQueue.pop();
	}
	int height = currBoard->getHeight();
	int width = currBoard->getWidth();
	gameTile* tileIterator;
	for (int i = 1; i <= height; i++) {
		for (int j = 1; j <= width; j++) {
			tileIterator = (*currBoard)(i, j);
			if (tileIterator->getState() == 'E') {
				pushTile.r = i;
				pushTile.c = j;
				pushTile.tile = tileIterator;
				resetQueue.push(pushTile);
				possMineLocs.push_back(pushTile);
			}
		}
	}
}

void gameSolver::resetRemainder() {
	int height = currBoard->getHeight();
	int width = currBoard->getWidth();
	solverTile tileIterator;
	int minesRemoved;
	int vPotMineIndex;
	gameTile* mineTile;
	while (!resetQueue.empty()) {
		tileIterator = resetQueue.front();
		resetQueue.pop();
		if (tileIterator.tile->isMine()) {
			tileIterator.tile->removeMine();
			minesRemoved++;
		}
		while (minesRemoved != 0) {
			vPotMineIndex = rand() % (height * width);
			int iH = (vPotMineIndex / height) + 1;
			int iW = (vPotMineIndex % width) + 1;
			mineTile = ((*currBoard)(iH,iW));
			if (mineTile->isMine() == 0) {
				mineTile->setMine();
				currBoard->doOnAllAdj(&gameTile::incAdjMines, iH, iW);
				minesRemoved--;
			}
		}
	}
}

void gameSolver::solverFlag(solverTile& target) {
	int r = target.r;
	int c = target.c;
	currBoard->setFlagOnTile(r, c);
}

void gameSolver::solverPress(solverTile& target) {
	int r = target.r;
	int c = target.c;	
	currBoard->pressTile(r, c);
	addAdjToSQueue(target);
}

void gameSolver::fillSQueue() {
	int height = currBoard->getHeight() + 1;
	int width = currBoard->getWidth() + 1;
	
	solverTile target;
	for (int i = 0; i <= height; i++) {
		for (int j = 0; j <= width; j++) {
			if ((*currBoard)(i, j)->getState() == 'E'){
				target.r = i;
				target.c = j;
				target.tile = (*currBoard)(i, j);
				if (peekForAdjPressed(target)){
					std::cerr << "Pushing tile:[" << i <<"][" << j << "].\n";
					solveQueue.push(target);
				}
			}
		}
	}

}

bool gameSolver::peekForAdjPressed(solverTile& target) {
	int height = currBoard->getHeight() + 1;
	int width = currBoard->getWidth() + 1;
	int r = target.r;
	int c = target.c;
	char state;
	for (signed int i = -1; i <= 1; i++) {
		for (signed int j = -1; j <= 1; j++) {
			if (i == 0 && j == 0) {
				continue;
			}
			else if (r + i < 0 || r + i > height || c + j < 0 || c + j > width) {
				continue;
			}
			else{
				state = (*currBoard)(r + i, c + j)->getState();
				if (state == 'P' || state == 'C'){
					return true;
				}
			}
		}
	}
	return false;
}

void gameSolver::addAdjToSQueue(solverTile& target) {
	int height = currBoard->getHeight() + 1;
	int width = currBoard->getWidth() + 1;
	solverTile pushTile;
	int r = target.r;
	int c = target.c;
	
	for (signed int i = -1; i <= 1; i++) {
		for (signed int j = -1; j <= 1; j++) {
			std::cerr << "Tile[" << r + i << "][" << c + j << "]"; 
			if (i == 0 && j == 0) {
				std::cerr << "is the center tile; ignoring.\n";
				continue;
			}
			else if (r + i < 0 || r + i > height || c + j < 0 || c + j > width) {
				std::cerr << "does not exist; ignoring.\n";
				continue;
			}
			else if ((*currBoard)(r + i, c + j)->getState() == 'E') {
				std::cerr << "has not been solved yet; pushing into queue.\n";
				pushTile.r = r + i;
				pushTile.c = c + j;
				pushTile.tile = (*currBoard)(r + i, c + j);
				solveQueue.push(pushTile);
			}
			else if ((*currBoard)(r + i, c + j)->getState() == 'F') {
				std::cerr << "has been flagged; ignoring.\n";
				continue;
			}
			else if ((*currBoard)(r + i, c + j)->getState() == 'C') {
				std::cerr << "has been cleared; ignoring.\n";
				continue;
			}
			else if ((*currBoard)(r + i, c + j)->getState() == 'P') {
				std::cerr << "has been pressed; ignoring.\n";
				continue;
			}
		}
	}
}

void gameSolver::removeMineOnTile(solverTile& target) {
	int r = target.r;
	int c = target.c;
	if ((*currBoard)(r, c)->getState() == 'M') {
		(*currBoard)(r, c)->removeMine();
		currBoard->doOnAllAdj(&gameTile::decAdjMines, r, c);
		return;
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