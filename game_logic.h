#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <queue>

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
#define MINESWEEPER_LIVES 3

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
};

class gameBoard: public boardOptions{
public:
	// ctor & dtor
	gameBoard();
	gameBoard(boardOptions& currSettings);
	gameBoard(boardOptions& currSettings, int seed);
	gameBoard(gameBoard& repeatBoard);
	~gameBoard();

	void pressTile(int r, int c);
	void setBorder();
	void unsetBoard();
	void unsetTile(int r, int c);
	void removeMineOnTile(int r, int c);
	void setFlagOnTile(int r, int c);
	void unsetFlagOnTile(int r, int c);
	void setMines();
	void printBoard(std::ostream& output);
	void printCheat(std::ostream& output);
	bool playBoard();
	void doOnAllAdj(void (gameBoard::* fncptr)(int, int), int r, int c);
	void doOnAllAdj(void (gameTile::*fncptr)(), int r, int c);
	gameTile* operator()(int r, int c);

private:
	gameTile* tileMatrix;
	gameTile** matrixAccessor;
	int safeTilesLeft;
	int seedVal;
	int minesPressed;
};

class gameSolver {
public:
	gameSolver(gameBoard& initBoard);
	void solveTile(int r, int c);
	void fillReset();
	void resetRemainder();
	void solveBoard();
	void solverFlag(int r, int c);
	void solverPress(int r, int c);
	void fillSQueue(int r, int c);
	void addAdjToSQueue(int r, int c);

private:
	gameBoard& currBoard;
	queue<gameTile*> solveQueue;
	queue<gameTile*> resetQueue;
	vector<gameTile*> possMineLocs;
	int remMines;
};
