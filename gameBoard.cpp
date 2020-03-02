#include "game_logic.h"

/* This next block of code are the various constructors and the destructor for the
** gameBoard class. the gameBoard is a class that is made of a 2D matrix of tiles which is
** dynamically generated based off of the dimensions set in boardOptions. It will use a
** random generator to place the mines, the # of mines not placed on the map yet is reduced
** to 0. The seed value, which will usually be obtained from time will be stored inside
** a variable, seedVal. It will be possible for the user, to prompt to start a game with
** a given seed as well.
** During the construction, height and width will always be given a value +2 of
** what is given by the getter functions to the boardOptions. This is to account for the
** size of the 'free border' that is provided to the players.
** Similarly, whenever a different function needs to know the height and width of the board,
** height and width will be given a value +1 of what is given by the getter
** functions, to account for the presence of the 0th index being the border.
** The only exception to this will be the SetMine function, as the border is meant to be
** completely mine-free.
*/

gameBoard::gameBoard() {
	seedVal = time(NULL);
	srand(seedVal);
	minesPressed = 0;
	int height = getHeight() + 2;
	int width = getWidth() + 2;
	tileMatrix = new gameTile[height * width];
	matrixAccessor = new gameTile * [height];
	if (matrixAccessor) {
		if (tileMatrix) {
			for (int i = 0; i < height; i++) {
				matrixAccessor[i] = &(tileMatrix[i * width]);
			}
			setMines();
			safeTilesLeft = height * width - getMines();
			setBorder();
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
	gameSolver solve(this);
}

gameBoard::gameBoard(boardOptions& currSettings) {
	setOpt(currSettings.getWidth(), currSettings.getHeight(), currSettings.getMines());
	seedVal = time(NULL);
	srand(seedVal);
	minesPressed = 0;
	int height = getHeight() + 2;
	int width = getWidth() + 2;
	tileMatrix = new gameTile[height * width];
	matrixAccessor = new gameTile * [height];
	if (matrixAccessor) {
		if (tileMatrix) {
			for (int i = 0; i < height; i++) {
				matrixAccessor[i] = &(tileMatrix[i * width]);
			}
			setMines();
			safeTilesLeft = height * width - getMines();
			setBorder();
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
	gameSolver solve(this);
	solve.fillSQueue();
	solve.solveBoard();
}

gameBoard::gameBoard(boardOptions& currSettings, int seed) {
	setOpt(currSettings.getWidth(), currSettings.getHeight(), currSettings.getMines());
	seedVal = seed;
	srand(seedVal);
	minesPressed = 0;
	int height = getHeight() + 2;
	int width = getWidth() + 2;
	tileMatrix = new gameTile[height * width];
	matrixAccessor = new gameTile * [height];
	std::cerr << "Sucessfully allocated the tiles\n";
	/* The following lines is a cheat made for the purposes of debugging */
	ofstream debugCheat;
	debugCheat.open("mineBoard.txt");
	/* */
	if (matrixAccessor) {
		if (tileMatrix) {
			for (int i = 0; i < height; i++) {
				matrixAccessor[i] = &(tileMatrix[i * width]);
			}
			std::cerr << "Setting Mines \n";
			setMines();	
			std::cerr << "Set Mines\n";
			safeTilesLeft = height * width - getMines();
			std::cerr << "Setting Borders\n";
			setBorder();
			std::cerr << "Set Borders\n";
			
			/* The following lines is a cheat made for the purposes of debugging */
			if (debugCheat.is_open()) {
				debugCheat << "Pre-Solver Board: \n";
				printCheat(debugCheat);
			}
			else {
				// Blank default else block
			}
			/* */
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
	gameSolver solve(this);
	solve.fillSQueue();
	solve.solveBoard();
	/* The following lines is a cheat made for the purposes of debugging */
	if (debugCheat.is_open()) {
		debugCheat << "Post-Solver Board: \n";
		printCheat(debugCheat);
		debugCheat.close();
	}
	else {
		// Blank default else block
	}
	/* */
}


gameBoard::gameBoard(gameBoard& repeatBoard){
	setOpt(repeatBoard.getWidth(), repeatBoard.getHeight(), repeatBoard.getMines());
	seedVal = repeatBoard.seedVal;
	minesPressed = 0;
	int height = getHeight() + 2;
	int width = getWidth() + 2;
	tileMatrix = new gameTile[height * width];
	matrixAccessor = new gameTile * [height];
	if (matrixAccessor) {
		if (tileMatrix) {
			for (int i = 1; i < height; i++) {
				matrixAccessor[i] = &(tileMatrix[i * width]);
			}
			for (int i = 1; i < height; i++) {
				for (int j = 1; j < width; j++) {
					matrixAccessor[i][j] = repeatBoard.matrixAccessor[i][j];
				}
			}
			safeTilesLeft = height * width - getMines();
			setBorder();
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
}

gameBoard::~gameBoard() {
	delete tileMatrix;
	delete matrixAccessor;
}

void gameBoard::pressTile(int r, int c) {
	char state = matrixAccessor[r][c].getState();
	bool blank = !matrixAccessor[r][c].getAdjMines();
	void (gameBoard:: * fncPtr)(int, int);
	fncPtr = &gameBoard::pressTile;
	if (state == 'P') {
		if (matrixAccessor[r][c].getRemMines()) {
			return;
		}
		else {
			matrixAccessor[r][c].setPressed();
			doOnAllAdj(fncPtr, r, c);
		}
	}
	else if(state == 'F' || state == 'C') {
		return;
	}
	else {
		matrixAccessor[r][c].setPressed();
		if (matrixAccessor[r][c].isMine()) {
			minesPressed++;
		}
		else {
			safeTilesLeft--;
		}
		doOnAllAdj(&gameTile::decAdjTile, r, c);
	}
	if (blank) {
		doOnAllAdj(fncPtr, r, c);
	}
}

void gameBoard::setBorder() {
	int height = getHeight() + 1;
	int width = getWidth() + 1;
	for (int i = 0; i <= height; i++) {
		if (i == 0 || i == height) {
			matrixAccessor[i][0].setAdjTile(3);
			matrixAccessor[i][width].setAdjTile(3);
		}
		else {
			matrixAccessor[i][0].setAdjTile(5);
			matrixAccessor[i][width].setAdjTile(5);
		}
		pressTile(i, 0);
		pressTile(i, width);
	}
	for (int j = 1; j < width; j++) {
		pressTile(0, j);
		pressTile(height, j);
		matrixAccessor[0][j].setAdjTile(5);
		matrixAccessor[height][j].setAdjTile(5);
	}
}

void gameBoard::setMines() {
	int height = getHeight();
	int width = getWidth();
	int vUnsetMinesInt = getMines();
	int vPotMineIndex = 0;
	
	int lAxis = max(height,width); // longer axis length
	int sAxis = min(height,width); // shorter axis length
	
	int iH;
	int iW;
	
	gameTile* currTile;
	while (vUnsetMinesInt != 0) {
		vPotMineIndex = rand() % (lAxis * sAxis);
		if(height >= width){
			iH = (vPotMineIndex / sAxis) + 1;
			iW = (vPotMineIndex % sAxis) + 1;
		}
		else{
			iW = (vPotMineIndex / sAxis) + 1;
			iH = (vPotMineIndex % sAxis) + 1;
		}
		currTile = &(matrixAccessor[iH][iW]);
		if (currTile->isMine() == 0) {
			currTile->setMine();
			doOnAllAdj(&gameTile::incAdjMines, iH, iW);
			vUnsetMinesInt--;
		}
	}
}

void gameBoard::unsetBoard() {
	int height = getHeight() + 1;
	int width = getWidth() + 1;
	for (int i = 1; i < height; i++) {
		for (int j = 1; j < width; j++) {
			unsetTile(i, j);
		}
	}
}

void gameBoard::unsetTile(int r, int c) {
	char state = matrixAccessor[r][c].getState();
	switch (state) {
	case 'F':
		matrixAccessor[r][c].unset();
		doOnAllAdj(&gameTile::incRem, r, c);
		break;
	case 'P':
	case 'C':
		matrixAccessor[r][c].unset();
		doOnAllAdj(&gameTile::incAdjTile, r, c);
		break;
	case 'Q':
		matrixAccessor[r][c].unset();
	default:
		// Empty Default Else Block
		break;
	}
	
}

void gameBoard::printBoard(std::ostream& output) {
	int height = getHeight() + 1;
	int width = getWidth() + 1;
	gameTile* targetTile;
	char tileState;
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
			targetTile = &(matrixAccessor[i][j]);
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
					output << std::setw(3) << std::left << targetTile->getAdjMines();
				}
			}

		}
		output << "\n";
	}
	output << "\n\n";
}

void gameBoard::printCheat(std::ostream& output) {
	int height = getHeight() + 1;
	int width = getWidth() + 1;
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
			gameTile* targetTile;
			targetTile = &(matrixAccessor[i][j]);
			if (targetTile->isMine()) {
				output << std::setw(3) << std::left << "M";
			}
			else {
				output << std::setw(3) << std::left << targetTile->getAdjMines();
			}
		}
		output << "\n";
	}
	output << "\n\n";
}

bool gameBoard::playBoard() {
	int height = getHeight() + 1;
	int width = getWidth() + 1;
	int r;
	int c;
	char vPlayerChoice;
	bool bValidInput = 0;
	char targetState;
	string playerInput;


	while (true) {
		if (safeTilesLeft == 0) {
			std::cout << "You have won the game! \n";
			return 0;
		}
		if (minesPressed > 3) {
			std::cout << "You have lost the game! \n";
			return 1;
		}

		bValidInput = 0;
		while (bValidInput == 0) {
			std::cout << "(F)lag or (P)ress? ";
			std::cin >> playerInput;
			vPlayerChoice = playerInput[0];
			vPlayerChoice = toupper(vPlayerChoice);
			// This switch block is made here despite being easier to implement as a if
			// block, to make the logic below easier.
			switch (vPlayerChoice)
			{
			case 'F':
			case 'P':
				bValidInput = 1;
				break;
			default:
				std::cout << "Invalid response; re-submit\n";
				break;
			}
		}

		bValidInput = 0;
		while (bValidInput == 0) {
			std::cout << "Input row number of next tile: ";
			std::cin >> playerInput;
			r = stoi(playerInput);
			if (r > 0 && r < height) {
				bValidInput = 1;
			}
			else {
				std::cout << "Invalid response; re-submit\n";
			}
		}

		bValidInput = 0;
		while (bValidInput == 0) {
			std::cout << "Input column number of next tile: ";
			std::cin >> playerInput;
			c = stoi(playerInput);
			if (c > 0 && c < width) {
				bValidInput = 1;
			}
			else {
				std::cout << "Invalid response; re-submit\n";
			}
		}

		switch (vPlayerChoice)
		{
		case 'F':
			targetState = matrixAccessor[r][c].getState();
			switch (targetState) {
			case 'F':
				unsetFlagOnTile(r, c);
				break;
			case 'P':
			case 'C':
				break;
			case 'E':
				setFlagOnTile(r, c);
				break;
			default:
				break;
			}
		case 'P':
			pressTile(r, c);
			break;
		default:
			// Only P and F can be inputted, and the check already occurred above.
			break;
		}

		printBoard(std::cout);
	}
}


void gameBoard::setFlagOnTile(int r, int c) {
	if (matrixAccessor[r][c].getState() == 'E') {
		matrixAccessor[r][c].setFlag();
		doOnAllAdj(&gameTile::decRem, r, c);
		return;
	}
}

void gameBoard::unsetFlagOnTile(int r, int c) {
	if (matrixAccessor[r][c].getState() == 'F') {
		matrixAccessor[r][c].unset();
		doOnAllAdj(&gameTile::incRem, r, c);
		return;
	}
}

gameTile* gameBoard::operator()(int r, int c) {
	return &(matrixAccessor[r][c]);
}

void gameBoard::doOnAllAdj(void (gameBoard::*fncptr)(int, int), int r, int c) {
	int height = getHeight() + 1;
	int width = getWidth() + 1;
	for (signed int i = -1; i <= 1; i++) {
		for (signed int j = -1; j <= 1; j++) {
			if (i == 0 && j == 0) {
				continue;
			}
			if (r + i < 0 || r + i > height || c + j < 0 || c + j > width) {
				continue;
			}
			else {
				(this->*fncptr)(r + i, c + j);
			}
		}
	}
}

void gameBoard::doOnAllAdj(void (gameTile::* fncptr)(), int r, int c) {
	int height = getHeight() + 1;
	int width = getWidth() + 1;
	for (signed int i = -1; i <= 1; i++) {
		for (signed int j = -1; j <= 1; j++) {
			if (i == 0 && j == 0) {
				continue;
			}
			if (r + i < 0 || r + i > height || c + j < 0 || c + j > width) {
				continue;
			}
			else {
				(matrixAccessor[r + i][c + j].*fncptr)();
			}
		}
	}
}