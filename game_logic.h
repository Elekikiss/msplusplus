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
	void setChained();
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
	void pressTileWOChain(int r, int c); // This version of pressTile exists only for use with the boardSolver;
	void setBorder();
	int getSafeTiles() {return safeTilesLeft;};
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
	int getAdjTiles(){return tile->getAdjTiles();};
	int getRemMines(){return tile->getRemMines();};
	char getState(){return tile->getState();};
	
	gameTile* tile;
	int r;
	int c;
};

/* The goal of this class is to solve to a reasonable degree the minesweeper board, and replacing all 
** undiscovered mines until the minesweeper board is no longer solvable by the algorithms that it is made
** with. It is not meant to solve *all* boards that can be solved with logic, but only to eliminate boards
** that cannot be solved with logic alone.
*/

/* Due to the poor readability of code each pattern of potential mine/safe-tile placement that one thinks of
** will be implemented as a separate function, which is called in order of commonness by the solver function
*/
class boardSolver {
public:
	boardSolver(gameBoard* initBoard);
	~boardSolver();
	void solveTile(solverTile& target);
	void solvePattern(solverTile& target);
	void recursiveAdjSolve(solverTile& target);
	void resetRemainder();
	void solveBoard();
	void solverFlag(solverTile& target);
	void solverRSFlag(solverTile& target);
	void solverFlagAllAdj(solverTile& target);
	void removeMineOnTile(solverTile& target);
	bool peekForAdjPressed(solverTile& target);
	bool peekForAdjEmpty(solverTile& target);
	void flagAdjExcShared(solverTile& target, solverTile& constraint);
	void pressAdjExcShared(solverTile& target, solverTile& constraint);
	
	int countSharedFree(solverTile& tileA, solverTile& tileB);
	
	/* Solver Patterns */
	bool testPattLargeAdjSmall(solverTile& target);
	bool testPatt131Corner(solverTile& target);

private:
	gameBoard* currBoard;
	bool bProgress;
	solverTile** sMatrixAccessor;
	solverTile* sTileMatrix;
};
