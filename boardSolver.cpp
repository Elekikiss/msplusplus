#include "game_logic.h"

// Ctor(s) and Dtor

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
}	// End of Ctor



boardSolver::~boardSolver(){
	delete sMatrixAccessor;
	delete sTileMatrix;
}	// End of Dtor



// End of Ctors and Dtors =============================================================================================



// Driver Functions

void boardSolver::solveBoard() {
	int height = currBoard->getHeight() + 1;
	int width = currBoard->getWidth() + 1;
	solverTile target;
	bProgress = false;
	bool bProgSinceReset = false;
	
	ofstream demoText;
	demoText.open("demoBoardSolving.txt");
	if (demoText.is_open()){
		demoText << "Board seed = " << currBoard->getSeedVal() << endl;
		currBoard->printBoard(demoText);
	}
	
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
							if(demoText.is_open()) currBoard->printBoard(demoText);
						}
					}
				}
			}
			if (bProgress == true) bProgSinceReset = true;
		}while (bProgress == true);
		
		if (resetRemainder(bProgSinceReset) == 0){
			break;
		}
	}
	
	if(demoText.is_open()) demoText.close();
}



void boardSolver::solveTile(solverTile& target) {
	char state = target.getState();
	/* Solver Logic Begins */
	if (state == 'E' || state == 'F' || state == 'C'){
		return;
	}
	if (target.getAdjTiles() == 0) {
		target.tile->setCleared();
		return;
	}
	/* Minesweeper Solution Logic Type 0: Nearby Tile Cleared */
	if (target.getRemMines() == 0){
		/* 0.1 Type No Remaining Mines */
		solverPressAllAdj(target);
		return;
	}
	else if (target.getRemMines() == target.getAdjTiles()){
		/* Type 0.2 Type Only Mines Remaining */
		solverFlagAllAdj(target);
		return;
	}
	
	solvePattern(target);
}



void boardSolver::solvePattern(solverTile& target){
	/* Logic Type 0 End */
	if(testPattSharedAdjs(target) == true){
		return;
	}
	if(testPatt131Corner(target) == true){
		return;
	}
	
	if(testPatt121Island(target) == true){
		return;
	}
}



// End of Driver Functions ============================================================================================



// Resetters

// The following function holds (most) of the logic required to fix a board that cannot be solved.
// Some of the helper functions within the class may be necessary for this function, hence the "(most)"

int boardSolver::resetRemainder(bool bProgSinceReset) {
	int height = currBoard->getHeight();
	int width = currBoard->getWidth();
	solverTile target;
	int minesToPlace = 0;
	int minesRemoved = 0;
	int maxMineIndex = 0;
	int vPotMineIndex = 0;
	char state;
	
	
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
						removeMineOnTile(target);
						minesRemoved++;
					}
					else if (bProgSinceReset == false){
						// In the event that the board is unsolvable, and there are no viable "0-adjacent-mine" tiles
						// to move a mine to (or in the slightly more likely event that RNG makes it seem to the program it as such)
						// a 5x5 square around the un-discoverable mine will have all of its mines removed in an attempt to make
						// the board possible to solve.
						minesRemoved += removeMineSquare(target, 5);
					}
				}
				else if (peekForAdjAllMines(target) && peekForAdjEmpty(target)){
					minesRemoved += removeMineSquare(target, 3);
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
	
	while (minesToPlace != 0) {
		vPotMineIndex = rand() % (maxMineIndex);
		target.r = possMineLocs[vPotMineIndex].r;
		target.c = possMineLocs[vPotMineIndex].c;
		target.tile = possMineLocs[vPotMineIndex].tile;
		if (target.tile->isMine() == 0) {
			target.tile->setMine();
			currBoard->doOnAllAdj(&gameTile::incAdjMines, target.r, target.c);
			minesToPlace--;
		}
	}
	return minesRemoved;
}



// The following functions are helper functions to resetRemainder that actually removes a mine from a tile

void boardSolver::removeMineOnTile(solverTile& target) {
	int r = target.r;
	int c = target.c;
	if ((*currBoard)(r, c)->isMine()) {
		(*currBoard)(r, c)->removeMine();
		currBoard->doOnAllAdj(&gameTile::decAdjMines, r, c);
		return;
	}
}



int boardSolver::removeMineSquare(solverTile& target, int sideLength) {
	int height = currBoard->getHeight() + 1;
	int width = currBoard->getWidth() + 1;
	int r = target.r;
	int c = target.c;
	int returnVal = 0;
	signed int indexMin = -((sideLength - 1)/2);
	signed int indexMax = ((sideLength - 1)/2);
	
	for (signed int i = indexMin; i <= indexMax; i++){
		for (signed int j = indexMin; j <= indexMax; j++){
			if (((r + i) > height) || ((r + i) < 0) || ((c + j) > width) || ((c + j) < 0)) continue;
			if ((*currBoard)(r + i, c + j)->isMine()) {
				(*currBoard)(r + i, c + j)->removeMine();
				currBoard->doOnAllAdj(&gameTile::decAdjMines, r + i, c + j);
				currBoard->doOnAllAdj(&gameBoard::unsetTile, r + i, c + j);
				returnVal++;
			}
		}
	}
	return returnVal;
}



// End of Resetters ===================================================================================================



// Recursion Helpers

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
			else if (((r + i) > height) || ((r + i) < 0) || ((c + j) > width) || ((c + j) < 0)) {
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
			else if (((r + i) > height) || ((r + i) < 0) || ((c + j) > width) || ((c + j) < 0)) {
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



void boardSolver::solverPressAllAdj(solverTile& target) {
	int height = currBoard->getHeight() + 1;
	int width = currBoard->getWidth() + 1;
	int r = target.r;
	int c = target.c;
	char state;
	solverTile pressTarget;
	queue<solverTile> recursionQueue;
	for (signed int i = -1; i <= 1; i++) {
		for (signed int j = -1; j <= 1; j++) {
			if (i == 0 && j == 0) {
				continue;
			}
			else if (((r + i) > height) || ((r + i) < 0) || ((c + j) > width) || ((c + j) < 0)) {
				continue;
			}
			else{
				state = sMatrixAccessor[r + i][c + j].getState();
				if (state == 'E'){
					pressTarget.r = r + i;
					pressTarget.c = c + j;
					pressTarget.tile = sMatrixAccessor[r + i][c + j].tile;
					solverPress(pressTarget);
					recursionQueue.push(pressTarget);
				}
			}
		}
	}
	while(!(recursionQueue.empty())){
		pressTarget = recursionQueue.front();
		solveTile(pressTarget);
		recursionQueue.pop();
	}
}



void boardSolver::solverRSFlag(solverTile& target) {
	solverFlag(target);
	recursiveAdjSolve(target);
}



void boardSolver::solverRSPress(solverTile& target) {
	solverPress(target);
	solveTile(target);
}


// End of Recursive Helpers ===========================================================================================



// Progressors (Non-Recursive)

// The following two functions are progressors that modifies the board progress and ticks the boolean condition.
// They are used mostly as a component to the RecursiveSolver (RS) version above.

void boardSolver::solverFlag(solverTile& target) {
	int r = target.r;
	int c = target.c;
	char state = target.tile->getState();
	if (state == 'E'){
		currBoard->setFlagOnTile(r, c);
		bProgress = true;
	}
}



void boardSolver::solverPress(solverTile& target){
	if (target.getState() == 'E'){
		currBoard->pressTile(target.r, target.c);
		bProgress = true;
	}
}



// These following two functions are also progressors, that imitate the logic of the "AllAdj" versions of
// the progressors above, but only for tiles that is not adjacent to a second tile, called the constraint.
// These functions are only called by the pattern solver algorithms; hence, they intentionally use non-recursive
// call makers to avoid runaway recursion caused by the pattern solvers, as pattern-based progression usually does not
// fully solve a tile.

void boardSolver::flagAdjExcShared(solverTile& focus, solverTile& constraint){
	int height = currBoard->getHeight() + 1;
	int width = currBoard->getWidth() + 1;
	int r = focus.r;
	int c = focus.c;
	solverTile target;
	for(signed int i = -1; i < 2; i++){
		for(signed int j = -1; j < 2; j++){
			if (((r + i) > height) || ((r + i) < 0) || ((c + j) > width) || ((c + j) < 0)) continue;
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
	int height = currBoard->getHeight() + 1;
	int width = currBoard->getWidth() + 1;
	int r = focus.r;
	int c = focus.c;
	solverTile target;
	for(signed int i = -1; i < 2; i++){
		for(signed int j = -1; j < 2; j++){
			if (((r + i) > height) || ((r + i) < 0) || ((c + j) > width) || ((c + j) < 0)) continue;
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
			solverPress(target);
		}
	}
}



// End of Progressors =================================================================================================



// Adjacent Tile Peekers
// These functions take a peek at adjacent tiles, to ascertain certain information that is needed to decide
// what the solver should do.

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
			else if (((r + i) > height) || ((r + i) < 0) || ((c + j) > width) || ((c + j) < 0)) {
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
			else if (((r + i) > height) || ((r + i) < 0) || ((c + j) > width) || ((c + j) < 0)) {
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
			else if (((r + i) > height) || ((r + i) < 0) || ((c + j) > width) || ((c + j) < 0)) {
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



int boardSolver::countSharedFree(solverTile& tileA, solverTile& tileB){
	int height = currBoard->getHeight() + 1;
	int width = currBoard->getWidth() + 1;
	int returnVal = 0;
	int r = tileA.r;
	int c = tileA.c;
	for(signed int i = -1; i < 2; i++){
		for(signed int j = -1; j < 2; j++){
			if (((r + i) > height) || ((r + i) < 0) || ((c + j) > width) || ((c + j) < 0)) continue;
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
				if ((*currBoard)(r + i, c + j)->getState() == 'E') returnVal++;
			}
		}
	}
	return returnVal;
}



// End of Peekers =====================================================================================================



// Pattern Solvers

// This is the most commonly seen pattern in minesweeper, and the most useful one to start simplifying a problem
// Whenever a large number is directly adjacent (or in a more advanced version of the logic, has a 1 tile gap between the two)
// to a tile with significantly less # of mines adj to it, it can often be used to glean useful information.
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
	
	minesA = sMatrixAccessor[r][c].getRemMines();
	adjForA = sMatrixAccessor[r][c].getAdjTiles();
	for (signed int i = -2; i < 3; i++){
		for(signed int j = -2; j < 3; j++){
			if (((r + i) > height) || ((r + i) < 0) || ((c + j) > width) || ((c + j) < 0)) continue;
			if (i == 0 && j == 0) continue;
			if (sMatrixAccessor[r + i][c + j].getState() == 'P'){
			
				/* The block of code inside this if block checks if there are mines in excess of the number 
				** of non-shared tiles. Between the two blocks being looked at (which are always no more 
				** than 1 tile apart from each other in any of the possible 8 directions) if the difference
				** of the non-shared adj tiles with max value for the remaining # of Adjacent mines between the two
				** is equal to the min value for the remaining # of Adjacent mines between the two:
				** It will flag all of the non-shared adj tiles for the appropriate tile, and press all
				** of the non-shared adj tiles for the other tile. Yeah. That was a mouthful alright.
				** Of note: This pattern covers both the common 121 and 1221 patterns.
				*/
				targetB = sMatrixAccessor[r + i][c + j];
				sharedAdj = countSharedFree(targetA, targetB);
				minesB = targetB.getRemMines();
				adjForB = targetB.getAdjTiles();
				if (sharedAdj < 2) continue;
				
				if (minesA == minesB){
					if (adjForA == sharedAdj){
						pressAdjExcShared(targetB, targetA);
						return true;
					}
					else if(adjForB == sharedAdj){
						pressAdjExcShared(targetA, targetB);
						return true;
					}
				}
				else if (minesA < minesB){
					if ((minesB - (adjForB - sharedAdj)) ==  minesA){
						flagAdjExcShared(targetB, targetA);
						pressAdjExcShared(targetA, targetB);
						return true;
					}
					else if ((adjForA == sharedAdj) && ((minesB - minesA) == (adjForB - sharedAdj))){
						flagAdjExcShared(targetB, targetA);
						return true;
					}
				}
				else if (minesA > minesB){
					if ((minesA - (adjForA - sharedAdj)) ==  minesB){
						flagAdjExcShared(targetA, targetB);
						pressAdjExcShared(targetB, targetA);
						return true;
					}
					else if((adjForB == sharedAdj) && ((minesA - minesB) == (adjForA - sharedAdj))){
						flagAdjExcShared(targetA, targetB);
						return true;
					}
				}
			}
		}
	}
	return false;
}



bool boardSolver::testPatt131Corner(solverTile& target){
	int height = currBoard->getHeight() + 1;
	int width = currBoard->getWidth() + 1;
	int r = target.r;
	int c = target.c;
	bool returnVal = false;
	solverTile oneTiles[2];
	
	int exitCondition = 0;
	if (r - 1 < 0 || r + 1 > height || c - 1 < 0 || c + 1 < width) return returnVal;
	
	if (sMatrixAccessor[r - 1][c - 1].getState() == 'F') exitCondition++;
	if (sMatrixAccessor[r + 1][c - 1].getState() == 'F') exitCondition++;
	if (sMatrixAccessor[r + 1][c + 1].getState() == 'F') exitCondition++;
	if (sMatrixAccessor[r - 1][c + 1].getState() == 'F') exitCondition++;
	if (exitCondition >= 3){
		return returnVal;
	}
	
	if(target.getRemMines() == 3 && target.getAdjTiles() == 5){
		if (sMatrixAccessor[r + 1][c + 1].getState() == 'P'){
			if((sMatrixAccessor[r + 0][c + 1].getState() == 'P') && (sMatrixAccessor[r + 1][c + 0].getState() == 'P')){
				if((sMatrixAccessor[r + 0][c + 1].getRemMines() == 1) && (sMatrixAccessor[r + 1][c + 0].getRemMines() == 1)){
					oneTiles[0].r = r + 1;
					oneTiles[0].c = c;
					oneTiles[0].tile = (*currBoard)(oneTiles[0].r, oneTiles[0].c);
					oneTiles[1].r = r;
					oneTiles[1].c = c + 1;
					oneTiles[1].tile = (*currBoard)(oneTiles[1].r, oneTiles[1].c);
					solverFlag(sMatrixAccessor[r - 1][c - 1]);
					pressAdjExcShared(oneTiles[0],target);
					pressAdjExcShared(oneTiles[1],target);
					returnVal = true;
				}
			}
			else
			{
				return returnVal;
			}
		}
		else if (sMatrixAccessor[r + 1][c - 1].getState() == 'P'){
			if((sMatrixAccessor[r + 0][c - 1].getState() == 'P') && (sMatrixAccessor[r + 1][c + 0].getState() == 'P')){
				if((sMatrixAccessor[r + 0][c - 1].getRemMines() == 1) && (sMatrixAccessor[r + 1][c + 0].getRemMines() == 1)){
					oneTiles[0].r = r;
					oneTiles[0].c = c - 1;
					oneTiles[0].tile = (*currBoard)(oneTiles[0].r, oneTiles[0].c);
					oneTiles[1].r = r + 1;
					oneTiles[1].c = c;
					oneTiles[1].tile = (*currBoard)(oneTiles[1].r, oneTiles[1].c);
					solverFlag(sMatrixAccessor[r - 1][c + 1]);
					pressAdjExcShared(oneTiles[0],target);
					pressAdjExcShared(oneTiles[1],target);
					returnVal = true;
				}
			}
			else
			{
				return returnVal;
			}
		}
		else if (sMatrixAccessor[r - 1][c - 1].getState() == 'P'){
			if((sMatrixAccessor[r + 0][c - 1].getState() == 'P') && (sMatrixAccessor[r - 1][c + 0].getState() == 'P')){
				if((sMatrixAccessor[r + 0][c - 1].getRemMines() == 1) && (sMatrixAccessor[r - 1][c + 0].getRemMines() == 1)){
					oneTiles[0].r = r - 1;
					oneTiles[0].c = c;
					oneTiles[0].tile = (*currBoard)(oneTiles[0].r, oneTiles[0].c);
					oneTiles[1].r = r;
					oneTiles[1].c = c - 1;
					oneTiles[1].tile = (*currBoard)(oneTiles[1].r, oneTiles[1].c);
					solverFlag(sMatrixAccessor[r - 1][c - 1]);
					pressAdjExcShared(oneTiles[0],target);
					pressAdjExcShared(oneTiles[1],target);
					returnVal = true;
				}
			}
			else
			{
				return returnVal;
			}
		}
		else if (sMatrixAccessor[r - 1][c + 1].getState() == 'P'){
			if((sMatrixAccessor[r + 0][c + 1].getState() == 'P') && (sMatrixAccessor[r - 1][c + 0].getState() == 'P')){
				if((sMatrixAccessor[r + 0][c + 1].getRemMines() == 1) && (sMatrixAccessor[r - 1][c + 0].getRemMines() == 1)){
					oneTiles[0].r = r - 1;
					oneTiles[0].c = c;
					oneTiles[0].tile = (*currBoard)(oneTiles[0].r, oneTiles[0].c);
					oneTiles[1].r = r;
					oneTiles[1].c = c + 1;
					oneTiles[1].tile = (*currBoard)(oneTiles[1].r, oneTiles[1].c);
					solverFlag(sMatrixAccessor[r - 1][c - 1]);
					pressAdjExcShared(oneTiles[0],target);
					pressAdjExcShared(oneTiles[1],target);
					returnVal = true;
				}
			}
			else
			{
				return returnVal;
			}
		}
	}
	return returnVal;
}



bool boardSolver::testPatt121Island(solverTile& target){
	int height = currBoard->getHeight() + 1;
	int width = currBoard->getWidth() + 1;
	int r = target.r;
	int c = target.c;
	
	if(!(target.getRemMines() == 2)) return false;
	
	solverTile newsTiles[4]; // news stands for North, East, West, South; the indexes are in that respective order.
	newsTiles[0].r = r - 1; 
	newsTiles[0].c = c;
	newsTiles[1].r = r;
	newsTiles[1].c = c + 1;
	newsTiles[2].r = r;
	newsTiles[2].c = c - 1;
	newsTiles[3].r = r + 1;
	newsTiles[3].c = c;
	if (((r - 1) < 0) ^ ((c + 1) > width) ^ ((c - 1) < 0) ^ ((r + 1) > height)){
		if (!((r - 1) < 0)) newsTiles[0].tile = (*currBoard)(newsTiles[0].r, newsTiles[0].c);
		else newsTiles[0].tile = nullptr;
		if (!((c + 1) > width)) newsTiles[1].tile = (*currBoard)(newsTiles[1].r, newsTiles[1].c);
		else newsTiles[1].tile = nullptr;
		if (!((c - 1) < 0)) newsTiles[2].tile = (*currBoard)(newsTiles[2].r, newsTiles[2].c);
		else newsTiles[2].tile = nullptr;
		if (!((r + 1) > height)) newsTiles[3].tile = (*currBoard)(newsTiles[3].r, newsTiles[3].c);
		else newsTiles[3].tile = nullptr;
	}
	else{
		return false;
	}
	
	if(newsTiles[0].tile != nullptr && newsTiles[3].tile != nullptr){
		if((newsTiles[0].getState() == 'P') && (newsTiles[3].getState() == 'P')){ 
			if((newsTiles[0].getRemMines() == 1) && (newsTiles[3].getRemMines() == 1)){
				if(newsTiles[1].tile != nullptr){
					solverPress(newsTiles[1]);
					if(newsTiles[1].getRemMines() == 1) pressAdjExcShared(newsTiles[1],target);
				}
				if(newsTiles[2].tile != nullptr){
					solverPress(newsTiles[2]);
					if(newsTiles[2].getRemMines() == 1) pressAdjExcShared(newsTiles[2],target);
				}
			}
			pressAdjExcShared(newsTiles[0],target);
			pressAdjExcShared(newsTiles[3],target);
			return true;
		}
	}
	else if(newsTiles[1].tile != nullptr && newsTiles[2].tile != nullptr){
		if((newsTiles[1].getState() == 'P') && (newsTiles[2].getState() == 'P')){ 
			if((newsTiles[1].getRemMines() == 1) && (newsTiles[2].getRemMines() == 1)){
				if(newsTiles[0].tile != nullptr){
					solverPress(newsTiles[0]);
					if(newsTiles[0].getRemMines() == 1) pressAdjExcShared(newsTiles[0],target);
				}
				if(newsTiles[3].tile != nullptr){
					solverPress(newsTiles[3]);
					if(newsTiles[3].getRemMines() == 1) pressAdjExcShared(newsTiles[3],target);
				}
			}
			pressAdjExcShared(newsTiles[1],target);
			pressAdjExcShared(newsTiles[2],target);
			return true;
		}
	}
	
	return false;
}



// End of Pattern Solvers =============================================================================================



// Debug Helpers
void boardSolver::solverPrint(std::ostream& output) {
	int height = currBoard->getHeight() + 1;
	int width = currBoard->getWidth() + 1;
	gameTile* targetTile;
	char tileState;
	
	output << "Remaining Mines Board\n";
	
	output << std::setfill(' ') << std::hex;
	output << "X  X  ";
	for (int j = 1; j < width; j++) {
		output << std::setw(3) << std::left << j;
	}
	output << "X  \n";
	for (int i = 0; i <= height; i++) {
		for (int j = 0; j <= width; j++) {
			if (j == 0) {
				if (i == 0 || i == height) output << "X  ";
				else output << std::setw(3) << std::left << i;
			}
			targetTile = (sMatrixAccessor[i][j].tile);
			tileState = targetTile->getState();
			switch (tileState) {
			case 'E':
			case 'F':
			case 'Q':
				output << std::setw(3) << std::left << tileState;
				break;
			case 'P':
			case 'C':
				if (targetTile->isMine()) {
					output << std::setw(3) << std::left << "M";
				}
				else {
					output << std::setw(3) << std::left << targetTile->getRemMines();
				}
			}

		}
		output << "\n";
	}
	output << "\n\n";
	
	output << "Remaining Free Tiles Board\n";
	
	output << std::setfill(' ') << std::hex;
	output << "X  X  ";
	for (int j = 1; j < width; j++) {
		output << std::setw(3) << std::left << j;
	}
	output << "X  \n";
	for (int i = 0; i <= height; i++) {
		for (int j = 0; j <= width; j++) {
			if (j == 0) {
				if (i == 0 || i == height) output << "X  ";
				else output << std::setw(3) << std::left << i;
			}
			targetTile = (sMatrixAccessor[i][j].tile);
			tileState = targetTile->getState();
			switch (tileState) {
			case 'E':
			case 'F':
			case 'Q':
				output << std::setw(3) << std::left << tileState;
				break;
			case 'P':
			case 'C':
				if (targetTile->isMine()) {
					output << std::setw(3) << std::left << "M";
				}
				else {
					output << std::setw(3) << std::left << targetTile->getAdjTiles();
				}
			}

		}
		output << "\n";
	}
	output << "\n\n";
}



// End of Debug Helpers ===============================================================================================