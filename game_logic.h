#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <queue>
#include <cmath>

using namespace std;

#define MINESWEEPER_EASY_WIDTH 9
#define MINESWEEPER_EASY_HEIGHT 9
#define MINESWEEPER_EASY_MINES 10
#define MINESWEEPER_MEDIUM_WIDTH 16
#define MINESWEEPER_MEDIUM_HEIGHT 16
#define MINESWEEPER_MEDIUM_MINES 40
#define MINESWEEPER_HARD_WIDTH 30
#define MINESWEEPER_HARD_HEIGHT 16
#define MINESWEEPER_HARD_MINES 99
#define MINESWEEPER_LIVES 1

class boardOptions {
public:
	// ctor & dtor
	boardOptions() {
		setOpt(MINESWEEPER_MEDIUM_WIDTH, MINESWEEPER_MEDIUM_HEIGHT, MINESWEEPER_MEDIUM_MINES);
	}
	// Uses default Copy Constructor.
	// Uses default destructor, as no dynamic allocation is used.

	// set and get
	int getWidth();
	int getHeight();
	int getMines();
	bool setOpt(int w, int h, int m);

private:
	int width;
	int height;
	int mines;
};

class gameTile {
public:
	gameTile();
	bool isMine();
	char getState();
	int getAdjMines();
	int getRemMines();
	int getAdjTiles();
	void setMine();
	void removeMine();
	void setPressed();
	void setFlag();
	void setQuestioning();
	void unset();
	void incAdjMines();
	void decAdjMines();
	void incRem();
	void decRem();
	void incAdjTile();
	void decAdjTile();
	void setAdjTile(int num);

private:
	bool bMine;
	char cState;
	int vAdjMineCount;
	int vRemAdjMines;
	int vFreeAdjTiles;
}; // The possible values of cState are: "(E)mpty, (C)lear, (P)ressed, (F)lagged, (Q)uestioning."
// Cleared means that it has already been chain pressed.

class gameBoard: public boardOptions{
public:
	// ctor & dtor
	gameBoard();
	gameBoard(boardOptions& currSettings);
	gameBoard(boardOptions& currSettings, int seed);
	gameBoard(gameBoard& repeatBoard);
	~gameBoard();

	void pressTile(int r, int c);
	void pressTileWOChain(int r, int c); // This version of pressTile exists only for use with the gameSolver;
	void setBorder();
	void unsetBoard();
	void unsetTile(int r, int c);
	void setMines();
	void printBoard(std::ostream& output);
	void printCheat(std::ostream& output);
	bool playBoard();
	void doOnAllAdj(void (gameBoard::* fncptr)(int, int), int r, int c);
	void doOnAllAdj(void (gameTile::*fncptr)(), int r, int c);
	void setFlagOnTile(int r, int c);
	void unsetFlagOnTile(int r, int c);
	gameTile* operator()(int r, int c);

private:
	gameTile* tileMatrix;
	gameTile** matrixAccessor;
	int safeTilesLeft;
	int seedVal;
	int minesPressed;
};

struct solverTile{
	gameTile* tile;
	int r;
	int c;
};

class gameSolver {
public:
	gameSolver(gameBoard* initBoard);
	void solveTile(solverTile& target);
	void recursiveAdjSolve(solverTile& target);
	void solveBoard();
	void solverFlag(solverTile& target);
	void gameSolver::solverFlagAllAdj(solverTile& target);
	void removeMineOnTile(solverTile& target);
	bool peekForAdjPressed(solverTile& target);

private:
	gameBoard* currBoard;
};
