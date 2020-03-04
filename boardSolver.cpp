#include "game_logic.h"

gameSolver::gameSolver(gameBoard* initBoard){
	currBoard = initBoard;
}

void gameSolver::solveBoard() {
	int height = currBoard->getHeight() + 1;
	int width = currBoard->getWidth() + 1;
	void (gameBoard:: * fncPtr)(int, int);
	solverTile target;
	do{
		
		for (int i = 0; i <= height; i++) {
			for (int j = 0; j <= width; j++) {
				if ((*currBoard)(i, j)->getState() == 'E'){
					target.r = i;
					target.c = j;
					target.tile = (*currBoard)(i, j);
					if (peekForAdjPressed(target)){
						std::cerr << "Starting Solver on tile:[" << i <<"][" << j << "].\n";
						solveTile(target);
					}
				}
			}
		}
		if (currBoard->safeTilesLeft()){
			
		}
	//} while (currBoard->safeTilesLeft())
	} while (false)
}

/* For the purposes of the solver, the indexes for the state goes like follows:
** 0 1 2
** 3 4 5
** 6 7 8
*/
void gameSolver::solveTile(solverTile& target) {
	int height = currBoard->getHeight() + 1;
	int width = currBoard->getWidth() + 1;
	int r = target.r;
	int c = target.c;
	char state[8] = {};
	int vAdjFreeTile[8] = {};
	int vRemainingMines[8] = {};
	solverTile kTile[8];
	int k = 0;
	
	if (target.tile->getState() == 'P' || target.tile->getFlag() == 'F' || target.tile->getState() == 'C'){
		return;
	}
	
	ofstream mineBoard;
	mineBoard.open("solverBoard.txt");
	if (mineBoard.is_open()) {
		printBoard(mineBoard);
		mineBoard.close();
	}
	
	for (signed int i = -1; i <= 1; i++) {
		for (signed int j = -1; j <= 1; j++) {
			if (i == 0 && j == 0) {
				vAdjFreeTile[k] = (*currBoard)(r + i, c + j)->getAdjTiles();
				vRemainingMines[k] = (*currBoard)(r + i, c + j)->getRemMines();
				state[k] = (*currBoard)(r + i, c + j)->getState();
				kTile[k].r = r + i;
				kTile[k].c = c + j;
				kTile[k].tile = (*currBoard)(r + i, c + j);
				continue;
			}
			else if (r + i < 0 || r + i > height || c + j < 0 || c + j > width) {
				continue;
			}
			else{
				vAdjFreeTile[k] = (*currBoard)(r + i, c + j)->getAdjTiles();
				vRemainingMines[k] = (*currBoard)(r + i, c + j)->getRemMines();
				state[k] = (*currBoard)(r + i, c + j)->getState();
				kTile[k].r = r + i;
				kTile[k].c = c + j;
				kTile[k].tile = (*currBoard)(r + i, c + j);
			}
			k++;
		}
	}
	
	/* Minesweeper Solution Logic Type Nearby Tile Cleared */
	for (int k = 0; k < 9; k++){
		if (k == 5) continue;
		if (state[k] == 'P') { // Type No Remaining Mines
			if (vRemainingMines[k] == 0){
				currBoard->doOnAllAdj(gameBoard::pressTileWOChain, kTile[k].r, kTile[k].c);
				recursiveAdjSolve(kTile[k]);
			}
			else if (vRemainingMines[k] == vAdjFreeTile [k]){
				solverFlagAllAdj(kTile[k]);
				return;
			}
		}
	}
	/* Comment Block Closer*/
	
	/* Minesweeper Solution Logic Type 1 2 1 */
	
	/* Comment Block Closer*/
	
	/* Minesweeper Solution Logic Type XXX */
	
	/* Comment Block Closer*/
	
	/* Minesweeper Solution Logic Type XXX */
	
	/* Comment Block Closer*/
	
	/* Minesweeper Solution Logic Type XXX */
	
	/* Comment Block Closer*/
}


/* This is a function that is designed to simplify the process of the recursive pressing; it simply does the job
** of recursively calling the solveTile function on all viable adjacent tiles to the tile passed as a reference.
*/
void recursiveAdjSolve(solverTile& target){
	solverTile nextTarget;
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
					continue;
				}
				else if (state == 'E'){
					nextTarget.r = r + i;
					nextTarget.c = c + j;
					nextTarget.tile = (*currBoard)(r + i, c + j);
					solveTile(nextTarget);
				}
			}
		}
	}

void gameSolver::resetRemainder() {
	int height = currBoard->getHeight();
	int width = currBoard->getWidth();
	solverTile target;
	int minesRemoved;
	signed int maxMineIndex = -1;
	int vPotMineIndex = 0;
	
	vector<solverTile> possMineLocs;
	for (int i = 1; i <= height; i++) {
		for (int j = 1; j <= width; j++) {
			if ((*currBoard)(i, j)->getState() == 'E'){
				target.r = i;
				target.c = j;
				target.tile = (*currBoard)(i, j);
				removeMineOnTile(target);
				possMineLocs.push_back(target);
				minesRemoved++;
				maxMineIndex++;
			}
			else if((*currBoard)(i, j)->getAdjMines() == 0){
				target.r = i;
				target.c = j;
				target.tile = (*currBoard)(i, j);
				possMineLocs.push_back(target);
				maxMineIndex++;
			}
		}
	}
	
	while (minesRemoved != 0) {
		vPotMineIndex = rand() % (maxMineIndex);
		target.r = possMineLocs[vPotMineIndex].r;
		target.c = possMineLocs[vPotMineIndex].c;
		target.tile = possMineLocs[vPotMineIndex].tile;
		if (currTile->isMine() == 0) {
			target.tile->setMine();
			currBoard->doOnAllAdj(&gameTile::incAdjMines, target.r, target.c);
			minesRemoved--;
		}
	}
}


void gameSolver::solverFlag(solverTile& target) {
	int r = target.r;
	int c = target.c;
	currBoard->setFlagOnTile(r, c);
}

void gameSolver::solverFlagAllAdj(solverTile& target) {
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
					continue;
				}
				else if (state == 'E'){
					currBoard->setFlagOnTile(r + i, c + j);
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

void gameSolver::removeMineOnTile(solverTile& target) {
	int r = target.r;
	int c = target.c;
	if ((*currBoard)(r, c)->isMine()) {
		(*currBoard)(r, c)->removeMine();
		currBoard->doOnAllAdj(&gameTile::decAdjMines, r, c);
		return;
	}
}


