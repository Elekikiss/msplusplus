#include "game_logic.h"

gameSolver::gameSolver(gameBoard* initBoard){
	currBoard = initBoard;
}

void gameSolver::solveBoard() {
	int height = currBoard->getHeight() + 1;
	int width = currBoard->getWidth() + 1;
	solverTile target;
	
	ofstream mineBoard;
	mineBoard.open("solverBoard.txt");
	
	currBoard->printBoard(mineBoard);
	
	//do{
		for (int i = 0; i <= height; i++) {
			for (int j = 0; j <= width; j++) {
				if ((*currBoard)(i, j)->getState() == 'E'){
					target.r = i;
					target.c = j;
					target.tile = (*currBoard)(i, j);
					if (peekForAdjPressed(target)){
						mineBoard << "Starting Solver on tile:[" << i <<"][" << j << "].\n";
						solveTile(target, mineBoard);
						mineBoard << "End of One Recursive Call Stack\n";
					}
				}
			}
		}
	//	if (currBoard->getSafeTiles()){
	//		resetRemainder();
	//	}
	//} while (currBoard->safeTilesLeft());
	
	mineBoard.close();
}

/* For the purposes of the solver, the indexes for the state goes like follows:
** 0 1 2
** 3 4 5
** 6 7 8
*/
//void gameSolver::solveTile(solverTile& target) {
void gameSolver::solveTile(solverTile& target, ofstream& sbsSolution) {
	int height = currBoard->getHeight() + 1;
	int width = currBoard->getWidth() + 1;
	int r = target.r;
	int c = target.c;
	char state[9] = {};
	int vAdjFreeTile[9] = {};
	int vRemainingMines[9] = {};
	solverTile kTile[9];
	
	if (target.tile->getState() == 'P' || target.tile->getState() == 'F' || target.tile->getState() == 'C'){
		return;
	}
	

	if (sbsSolution.is_open()) {
		currBoard->printBoard(sbsSolution);
	}
	
	int k = 0;
	for (signed int i = -1; i <= 1; i++) {
		for (signed int j = -1; j <= 1; j++, k++) {
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
				kTile[k].tile = nullptr;
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
		}
	}
	
	sbsSolution << "Solution Logics: Tile #0: state = " << state[0] << ", adjFree = " << vAdjFreeTile[0]
			  << ", remMines = " << vRemainingMines[0] << "\n";
	sbsSolution << "Solution Logics: Tile #1: state = " << state[1] << ", adjFree = " << vAdjFreeTile[1]
			  << ", remMines = " << vRemainingMines[1] << "\n";
	sbsSolution << "Solution Logics: Tile #2: state = " << state[2] << ", adjFree = " << vAdjFreeTile[2]
			  << ", remMines = " << vRemainingMines[2] << "\n";
	sbsSolution << "Solution Logics: Tile #3: state = " << state[3] << ", adjFree = " << vAdjFreeTile[3]
			  << ", remMines = " << vRemainingMines[3] << "\n";
	sbsSolution << "Solution Logics: Tile #4: state = " << state[4] << ", adjFree = " << vAdjFreeTile[4]
			  << ", remMines = " << vRemainingMines[4] << "\n";
	sbsSolution << "Solution Logics: Tile #6: state = " << state[6] << ", adjFree = " << vAdjFreeTile[6]
			  << ", remMines = " << vRemainingMines[6] << "\n";
	sbsSolution << "Solution Logics: Tile #7: state = " << state[7] << ", adjFree = " << vAdjFreeTile[7]
			  << ", remMines = " << vRemainingMines[7] << "\n";
	sbsSolution << "Solution Logics: Tile #8: state = " << state[8] << ", adjFree = " << vAdjFreeTile[8]
			  << ", remMines = " << vRemainingMines[8] << "\n";
	
	/* Minesweeper Solution Logic Type Nearby Tile Cleared */
	for (k = 0; k < 9; k++){
		if (k == 5) continue;
		if ((state[k] == 'P' || state[k] == 'C') && kTile[k].tile != nullptr) { // 0.1 Type No Remaining Mines
			if (vRemainingMines[k] == 0){
				currBoard->doOnAllAdj(&gameBoard::pressTileWOChain, kTile[k].r, kTile[k].c);
				sbsSolution << "Pattern 1 encountered, [" << kTile[k].r << "][" << kTile[k].c << "]\n";
				// recursiveAdjSolve(kTile[k]);
				recursiveAdjSolve(kTile[k], sbsSolution);
			}
			else if (vRemainingMines[k] == vAdjFreeTile [k]){ // Type 0.2 Type Only Mines Remaining
				sbsSolution << "Pattern 2 encountered, [" << kTile[k].r << "][" << kTile[k].c << "]\n";
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
//void gameSolver::recursiveAdjSolve(solverTile& target){
void gameSolver::recursiveAdjSolve(solverTile& target, ofstream& sbsSolution){
	sbsSolution << "Recursive Caller called\n";
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
					//solveTile(nextTarget);
					solveTile(nextTarget, sbsSolution);
				}
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
		if (target.tile->isMine() == 0) {
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


