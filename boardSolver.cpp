#include "game_logic.h"

boardSolver::boardSolver(gameBoard* initBoard){
	currBoard = initBoard;
	
	int height = currBoard->getHeight() + 2;
	int width = currBoard->getWidth() + 2;
	sTileMatrix = new solverTile[height * width];
	sMatrixAccessor = new solverTile * [height];
	if (sMatrixAccessor) {
		if (sTileMatrix) {
			for (int i = 0; i < height; i++) {
				sMatrixAccessor[i] = &(sTileMatrix[i * width]);
			}
		}
		else {
			std::cerr << ("Error has occured allocating memory for a board.\n");
			exit(1);
		}
	}
	else {
		std::cerr << ("Error has occured allocating memory for a board.\n");
		exit(1);
	}
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++){
			sMatrixAccessor[i][j].r = i;
			sMatrixAccessor[i][j].c = j;
			sMatrixAccessor[i][j].tile = (*currBoard)(i, j);
		}
	}
}

boardSolver::~boardSolver(){
	delete sMatrixAccessor;
	delete sTileMatrix;
}

void boardSolver::solveBoard() {
	int height = currBoard->getHeight() + 1;
	int width = currBoard->getWidth() + 1;
	solverTile target;
	bProgress = false;
	
	while(true){
		do{
			bProgress = false;
			for (int i = 0; i <= height; i++) {
				for (int j = 0; j <= width; j++) {
					if (sMatrixAccessor[i][j].getState() == 'P'){
						target.r = i;
						target.c = j;
						target.tile = (*currBoard)(i, j);
						if (peekForAdjEmpty(target)){
							solveTile(target);
						}
					}
				}
			}
			std::cerr << "bProgress = " << bProgress << "\n\n";
			currBoard->printBoard(std::cerr);
		}while (bProgress == true);
		
		if (resetRemainder() == 0){
			break;
		}
	}
}


void boardSolver::solveTile(solverTile& target) {
	int r = target.r;
	int c = target.c;
	
	/* Solver Logic Begins */
	std::cerr << "Solver logic: single tile begin\n";
	if (target.getState() == 'E' || target.getState() == 'F'){
		return;
	}
	if (target.getAdjTiles() == 0) {
		std::cerr << "Tile Fully solved\n";
		target.tile->setCleared();
		return;
	}
	/* Minesweeper Solution Logic Type 0: Nearby Tile Cleared */
	if (target.getRemMines() == 0){
		/* 0.1 Type No Remaining Mines */
		std::cerr << "Solver logic: no more mines\n";
		currBoard->doOnAllAdj(&gameBoard::pressTileWOChain, r, c);
		recursiveAdjSolve(target);
		bProgress = true;
		return;
	}
	else if (target.getRemMines() == target.getAdjTiles()){
		/* Type 0.2 Type Only Mines Remaining */
		std::cerr << "Solver logic: no more safe tiles\n";
		solverFlagAllAdj(target);
		bProgress = true;
		return;
	}
	
	std::cerr << "bProgress is still 0; 0 = " << bProgress << "\n";
	std::cerr << "Exiting single tile Solver and calling pattern based solver.\n";
	solvePattern(target);
}

void boardSolver::solvePattern(solverTile& target){
	/* Logic Type 0 End */
	std::cerr << "Solver logic: Patterns begin\n";
	if(testPattSharedAdjs(target) == true){
		bProgress = true;
		return;
	}
	if(testPatt131Corner(target) == true){
		bProgress = true;
		return;
	}
	std::cerr << "Solver logic: Patterns not found\n";
}


/* This is a function that is designed to simplify the process of the recursive pressing; it simply does the job
** of recursively calling the solveTile function on all viable adjacent tiles to the tile passed as a reference.
*/
void boardSolver::recursiveAdjSolve(solverTile& target){
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
				state = sMatrixAccessor[r + i][c + j].getState();
				if (state != 'P'){
					continue;
				}
				else if (state == 'P'){
					nextTarget.r = r + i;
					nextTarget.c = c + j;
					nextTarget.tile = sMatrixAccessor[r + i][c + j].tile;
					solveTile(nextTarget);
				}
			}
		}
	}
}

int boardSolver::resetRemainder() {
	int height = currBoard->getHeight();
	int width = currBoard->getWidth();
	solverTile target;
	int minesToPlace = 0;
	int minesRemoved = 0;
	int maxMineIndex = 0;
	int vPotMineIndex = 0;
	char state;
	
	std::cerr << "Board not solvable, at least with current pattern recognitions. Relocating mines...\n";
	vector<solverTile> possMineLocs;
	
	for (int i = 1; i <= height; i++) {
		for (int j = 1; j <= width; j++) {
			if ((*currBoard)(i, j)->getState() == 'E'){
				target.r = i;
				target.c = j;
				target.tile = (*currBoard)(i, j);
				possMineLocs.push_back(target);
				maxMineIndex++;
				if ((*currBoard)(i, j)->isMine()){
					if(!(peekForAdjAllMines(target))){
						std::cerr << "Removing Mine\n";
						removeMineOnTile(target);
						minesRemoved++;
					}
				}
			}
		}
	}
	
	if (minesRemoved == 0) return 0;
	for (int i = 1; i <= height; i++) {
		for (int j = 1; j <= width; j++) {
			state = (*currBoard)(i, j)->getState();
			if(state == 'P' || state == 'C'){
				target.r = i;
				target.c = j;
				target.tile = (*currBoard)(i, j);
				if ((*currBoard)(i, j)->getAdjMines() == 0){
					currBoard->unsetTile(i, j);
					currBoard->doOnAllAdj(&gameTile::setClearedToPressed, i, j);
					possMineLocs.push_back(target);
					maxMineIndex++;
				}
			}
		}
	}
	
	minesToPlace = minesRemoved;
	std::cerr << "Populated Potential Mine Vector; placing: " << minesToPlace << " Mines\n";
	
	while (minesToPlace != 0) {
		vPotMineIndex = rand() % (maxMineIndex);
		target.r = possMineLocs[vPotMineIndex].r;
		target.c = possMineLocs[vPotMineIndex].c;
		target.tile = possMineLocs[vPotMineIndex].tile;
		if (target.tile->isMine() == 0) {
			target.tile->setMine();
			currBoard->doOnAllAdj(&gameTile::incAdjMines, target.r, target.c);
			minesToPlace--;
			std::cerr << minesToPlace << " Mines left to place\n";
		}
	}
	return minesRemoved;
}


void boardSolver::solverFlag(solverTile& target) {
	int r = target.r;
	int c = target.c;
	currBoard->setFlagOnTile(r, c);
}


void boardSolver::solverRSFlag(solverTile& target) {
	solverFlag(target);
	recursiveAdjSolve(target);
}

void boardSolver::solverFlagAllAdj(solverTile& target) {
	int height = currBoard->getHeight() + 1;
	int width = currBoard->getWidth() + 1;
	int r = target.r;
	int c = target.c;
	char state;
	solverTile flagTarget;
	for (signed int i = -1; i <= 1; i++) {
		for (signed int j = -1; j <= 1; j++) {
			if (i == 0 && j == 0) {
				continue;
			}
			else if (r + i < 0 || r + i > height || c + j < 0 || c + j > width) {
				continue;
			}
			else{
				state = sMatrixAccessor[r + i][c + j].getState();
				if (state == 'E'){
					flagTarget.r = r + i;
					flagTarget.c = c + j;
					flagTarget.tile = sMatrixAccessor[r + i][c + j].tile;
					solverRSFlag(flagTarget);
				}
			}
		}
	}
}

bool boardSolver::peekForAdjPressed(solverTile& target) {
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
				state = sMatrixAccessor[r + i][c + j].getState();
				if (state == 'P' || state == 'C'){
					return true;
				}
			}
		}
	}
	return false;
}

bool boardSolver::peekForAdjEmpty(solverTile& target) {
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
				state = sMatrixAccessor[r + i][c + j].getState();
				if (state == 'E'){
					return true;
				}
			}
		}
	}
	return false;
}

bool boardSolver::peekForAdjAllMines(solverTile& target) {
	int height = currBoard->getHeight() + 1;
	int width = currBoard->getWidth() + 1;
	int r = target.r;
	int c = target.c;
	int mine = 0;
	for (signed int i = -1; i <= 1; i++) {
		for (signed int j = -1; j <= 1; j++) {
			if (i == 0 && j == 0) {
				continue;
			}
			else if (r + i < 0 || r + i > height || c + j < 0 || c + j > width) {
				continue;
			}
			else{
				if ((*currBoard)(r + i, c + j)->isMine()) mine++;
				if (mine == 8){
					return true;
				}
			}
		}
	}
	return false;
}

void boardSolver::removeMineOnTile(solverTile& target) {
	int r = target.r;
	int c = target.c;
	if ((*currBoard)(r, c)->isMine()) {
		(*currBoard)(r, c)->removeMine();
		currBoard->doOnAllAdj(&gameTile::decAdjMines, r, c);
		return;
	}
}


/* These following two functions are helper functions for the Shared Adjacency patterns;
** One might notice that these functions use versions of the code that will not/cannot make a recursive call
** to the tiles Adjacent to the tiles it either presses or flags. This is intentional, as iterative wrapper around
** the recursion function(s) will repeat itself unless no further progress have been made on the board.
** As the various forms of the Shared Adjacency pattern does not fully solve the tile, but instead merely add information,
** letting this function to make recursive calls will cause runaway recursion to occur.
*/
void boardSolver::flagAdjExcShared(solverTile& focus, solverTile& constraint){
	int r = focus.r;
	int c = focus.c;
	solverTile target;
	for(signed int i = -1; i < 2; i++){
		for(signed int j = -1; j < 2; j++){
			if (i == 0 && j == 0) continue;
			if(((((r + i) == (constraint.r - 1)) && ((c + j)) == (constraint.c - 1)))
			|| ((((r + i) == (constraint.r - 1)) && ((c + j)) == (constraint.c + 0)))
			|| ((((r + i) == (constraint.r - 1)) && ((c + j)) == (constraint.c + 1)))
			|| ((((r + i) == (constraint.r + 0)) && ((c + j)) == (constraint.c - 1)))
			|| ((((r + i) == (constraint.r + 0)) && ((c + j)) == (constraint.c + 0)))
			|| ((((r + i) == (constraint.r + 0)) && ((c + j)) == (constraint.c + 1)))
			|| ((((r + i) == (constraint.r + 1)) && ((c + j)) == (constraint.c - 1)))
			|| ((((r + i) == (constraint.r + 1)) && ((c + j)) == (constraint.c + 0)))
			|| ((((r + i) == (constraint.r + 1)) && ((c + j)) == (constraint.c + 1)))) continue;
			target.r = r + i;
			target.c = c + j;
			target.tile = sMatrixAccessor[r + i][c + j].tile;
			solverFlag(target);
		}
	}
}

void boardSolver::pressAdjExcShared(solverTile& focus, solverTile& constraint){
	int r = focus.r;
	int c = focus.c;
	for(signed int i = -1; i < 2; i++){
		for(signed int j = -1; j < 2; j++){
			if (i == 0 && j == 0) continue;
			if(((((r + i) == (constraint.r - 1)) && ((c + j)) == (constraint.c - 1)))
			|| ((((r + i) == (constraint.r - 1)) && ((c + j)) == (constraint.c + 0)))
			|| ((((r + i) == (constraint.r - 1)) && ((c + j)) == (constraint.c + 1)))
			|| ((((r + i) == (constraint.r + 0)) && ((c + j)) == (constraint.c - 1)))
			|| ((((r + i) == (constraint.r + 0)) && ((c + j)) == (constraint.c + 0)))
			|| ((((r + i) == (constraint.r + 0)) && ((c + j)) == (constraint.c + 1)))
			|| ((((r + i) == (constraint.r + 1)) && ((c + j)) == (constraint.c - 1)))
			|| ((((r + i) == (constraint.r + 1)) && ((c + j)) == (constraint.c + 0)))
			|| ((((r + i) == (constraint.r + 1)) && ((c + j)) == (constraint.c + 1)))) continue;
			currBoard->pressTileWOChain(r + i, c + j);
		}
	}
}

int boardSolver::countSharedFree(solverTile& tileA, solverTile& tileB){
	int returnVal = 0;
	int r = tileA.r;
	int c = tileA.c;
	for(signed int i = -1; i < 2; i++){
		for(signed int j = -1; j < 2; j++){
			if (i == 0 && j == 0) continue;
			if (((r + i) == (tileB.r + 0)) && ((c + j)) == (tileB.c + 0)) continue;
			if(((((r + i) == (tileB.r - 1)) && ((c + j)) == (tileB.c - 1)))
			|| ((((r + i) == (tileB.r - 1)) && ((c + j)) == (tileB.c + 0)))
			|| ((((r + i) == (tileB.r - 1)) && ((c + j)) == (tileB.c + 1)))
			|| ((((r + i) == (tileB.r + 0)) && ((c + j)) == (tileB.c - 1)))
			|| ((((r + i) == (tileB.r + 0)) && ((c + j)) == (tileB.c + 1)))
			|| ((((r + i) == (tileB.r + 1)) && ((c + j)) == (tileB.c - 1)))
			|| ((((r + i) == (tileB.r + 1)) && ((c + j)) == (tileB.c + 0)))
			|| ((((r + i) == (tileB.r + 1)) && ((c + j)) == (tileB.c + 1)))){
				returnVal++;
				continue;
			}
		}
	}
	return returnVal;
}

/* This is the most commonly seen pattern in minesweeper, and the most useful one to start simplifying a problem
** Whenever a large number is directly adjacent (or in a more advanced version of the logic, has a 1 tile gap between the two)
** to a tile with significantly less # of mines adj to it, it can often be used to glean useful information.
*/
bool boardSolver::testPattSharedAdjs(solverTile& targetA){
	
	
	int height = currBoard->getHeight() + 1;
	int width = currBoard->getWidth() + 1;
	int r = targetA.r;
	int c = targetA.c;
	solverTile targetB;
	int minesA;
	int minesB;
	int adjForA;
	int adjForB;
	int sharedAdj;
	bool returnVal = false;
	
	minesA = sMatrixAccessor[r][c].getRemMines();
	adjForA = sMatrixAccessor[r][c].getAdjTiles();
	for (signed int i = -2; i < 3; i++){
		for(signed int j = -2; j < 3; j++){
			if (r + i > height || r + i < 0 || c + j > width || c + j < 0) continue;
			if (i == 0 && j == 0) continue;
			if (sMatrixAccessor[r + i][c + j].getState() == 'P'){
				/* The block of code inside this if block checks if there are mines in excess of the number 
				** of non-shared tiles. Between the two blocks being looked at, which are always no more 
				** than 1 tile apart from each other in any of the possible 8 directions, if the difference
				** of the non-shared adj tiles with max value for the remaining # of Adjacent mines between the two
				** is equal to the min value for the remaining # of Adjacent mines between the two:
				** It will flag all of the non-shared adj tiles for the appropriate tile, and press all
				** of the non-shared adj tiles for the other tile. Yeah. That was a mouthful alright.
				** Of note: This pattern covers both the common 121 and 1221 patterns.
				*/
				targetB = sMatrixAccessor[r + i][c + j];
				sharedAdj = countSharedFree(targetA, targetB);
				if (sharedAdj < 2) continue;
				minesB = targetB.getRemMines();
				adjForB = targetB.getRemMines();
				
				/*
				if (minesA == minesB){
					if (adjForA == sharedAdj){
						pressAdjExcShared(targetB, targetA);
						returnVal = true;
					}
					else if(adjForB == sharedAdj){
						pressAdjExcShared(targetA, targetB);
						returnVal = true;
					}
				}
				*/
				/*
				else if (minesA < minesB){
					if (minesB - (adjForB - sharedAdj) ==  minesA){
						flagAdjExcShared(targetB, targetA);
						pressAdjExcShared(targetA, targetB);
						returnVal = true;
					}
				}
				*/
				/*
				else if (minesA > minesB){
					if (minesA - (adjForA - sharedAdj) ==  minesB){
						flagAdjExcShared(targetA, targetB);
						pressAdjExcShared(targetB, targetA);
						returnVal = true;
					}
				}
				*/
			}
		}
	}
	return returnVal;
}

bool boardSolver::testPatt131Corner(solverTile& target){
	int height = currBoard->getHeight() + 1;
	int width = currBoard->getWidth() + 1;
	int r = target.r;
	int c = target.c;
	bool returnVal = false;
	
	int exitCondition = 0;
	if (r - 1 < 0 || r + 1 > height || c - 1 < 0 || c + 1 < width) return returnVal;
	
	if (sMatrixAccessor[r - 1][c - 1].getState() == 'F') exitCondition++;
	if (sMatrixAccessor[r + 1][c - 1].getState() == 'F') exitCondition++;
	if (sMatrixAccessor[r + 1][c + 1].getState() == 'F') exitCondition++;
	if (sMatrixAccessor[r - 1][c + 1].getState() == 'F') exitCondition++;
	if (exitCondition >= 3){
		return returnVal;
	}
	
	if(sMatrixAccessor[r][c].getRemMines() == 3){
		if (sMatrixAccessor[r + 1][c + 1].getState() == 'P'){
			if((sMatrixAccessor[r + 0][c + 1].getState() == 'P') && (sMatrixAccessor[r + 1][c + 0].getState() == 'P')){
				if((sMatrixAccessor[r + 0][c + 1].getRemMines() == 1) && (sMatrixAccessor[r + 1][c + 0].getRemMines() == 1)){
					solverFlag(sMatrixAccessor[r - 1][c - 1]);
					solverFlag(sMatrixAccessor[r + 1][c - 1]);
					solverFlag(sMatrixAccessor[r - 1][c + 1]);
					returnVal = true;
					recursiveAdjSolve(sMatrixAccessor[r - 1][c - 1]);
					recursiveAdjSolve(sMatrixAccessor[r + 1][c - 1]);
					recursiveAdjSolve(sMatrixAccessor[r - 1][c + 1]);
				}
			}
		}
		else if (sMatrixAccessor[r + 1][c - 1].getState() == 'P'){
			if((sMatrixAccessor[r + 0][c - 1].getState() == 'P') && (sMatrixAccessor[r + 1][c + 0].getState() == 'P')){
				if((sMatrixAccessor[r + 0][c - 1].getRemMines() == 1) && (sMatrixAccessor[r + 1][c + 0].getRemMines() == 1)){
					solverFlag(sMatrixAccessor[r - 1][c - 1]);
					solverFlag(sMatrixAccessor[r + 1][c + 1]);
					solverFlag(sMatrixAccessor[r - 1][c + 1]);
					returnVal = true;
					recursiveAdjSolve(sMatrixAccessor[r - 1][c - 1]);
					recursiveAdjSolve(sMatrixAccessor[r + 1][c + 1]);
					recursiveAdjSolve(sMatrixAccessor[r - 1][c + 1]);
				}
			}
		}
		else if (sMatrixAccessor[r - 1][c - 1].getState() == 'P'){
			if((sMatrixAccessor[r + 0][c - 1].getState() == 'P') && (sMatrixAccessor[r - 1][c + 0].getState() == 'P')){
				if((sMatrixAccessor[r + 0][c - 1].getRemMines() == 1) && (sMatrixAccessor[r - 1][c + 0].getRemMines() == 1)){
					solverFlag(sMatrixAccessor[r + 1][c + 1]);
					solverFlag(sMatrixAccessor[r + 1][c - 1]);
					solverFlag(sMatrixAccessor[r - 1][c + 1]);
					returnVal = true;
					recursiveAdjSolve(sMatrixAccessor[r + 1][c + 1]);
					recursiveAdjSolve(sMatrixAccessor[r + 1][c - 1]);
					recursiveAdjSolve(sMatrixAccessor[r - 1][c + 1]);
				}
			}
		}
		else if (sMatrixAccessor[r - 1][c + 1].getState() == 'P'){
			if((sMatrixAccessor[r + 0][c + 1].getState() == 'P') && (sMatrixAccessor[r - 1][c + 0].getState() == 'P')){
				if((sMatrixAccessor[r + 0][c + 1].getRemMines() == 1) && (sMatrixAccessor[r - 1][c + 0].getRemMines() == 1)){
					solverFlag(sMatrixAccessor[r - 1][c - 1]);
					solverFlag(sMatrixAccessor[r + 1][c - 1]);
					solverFlag(sMatrixAccessor[r + 1][c + 1]);
					returnVal = true;
					recursiveAdjSolve(sMatrixAccessor[r - 1][c - 1]);
					recursiveAdjSolve(sMatrixAccessor[r + 1][c - 1]);
					recursiveAdjSolve(sMatrixAccessor[r + 1][c + 1]);
				}
			}
		}
	}
	return returnVal;
}
