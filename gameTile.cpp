#include "game_logic.h"

gameTile::gameTile() {
	bMine = 0;
	cState = 'E';
	vAdjMineCount = 0;
	vFreeAdjTiles = 8;
}

bool gameTile::isMine() {
	return bMine;
}

char gameTile::getState() {
	switch (cState) {
	case 'F': // Flagged
	case 'E': // Empty
	case 'P': // Pressed
	case 'Q': // Question-mark flag
	case 'C': // Cleared
		return cState;
		break;
	default:
		// Do something to let the program know a fatal error has occurred and
		// either exit the program, or clear the tile's state. This should 
		// never happen without some sort of memory editing or corruption occurring.
		exit(1);
		break;
	}
}

/* This next block of code is the get/set functions for the gameTile Class.
** For most of the setter functions, there is no input required, as there is a function
** for each legal values of the various variables. The determination of which function
** will be called is managed entirely by a driver function that interacts with the gameboard
** and player interaction with it.
*/
int gameTile::getAdjMines() {
	return vAdjMineCount;
}

int gameTile::getRemMines() {
	return vRemAdjMines;
}

int gameTile::getAdjTiles () {
	return vFreeAdjTiles;
}

void gameTile::setMine() {
	bMine = 1;
}

void gameTile::removeMine() {
	bMine = 0;
}

void gameTile::setPressed() {
	if (cState == 'E' || cState == 'Q') {
		cState = 'P';
	}
	if (cState == 'P'){
		cState = 'C';
	}
}

void gameTile::setFlag() {
	if (cState == 'E' || cState == 'Q') {
		cState = 'F';
	}
}

void gameTile::unset() {
	cState = 'E';
}

void gameTile::setQuestioning() {
	if (cState == 'Q') {
		cState = 'E';
	}
	else if (cState == 'E') {
		cState = 'Q';
	}
}

void gameTile::incAdjMines() {
	incRem();
	vAdjMineCount++;
}

void gameTile::decAdjMines() {
	decRem();
	vAdjMineCount--;
}

void gameTile::incRem() {
	vRemAdjMines++;
}

void gameTile::decRem() {
	vRemAdjMines--;
}

void gameTile::incAdjTile() {
	vFreeAdjTiles++;
}

void gameTile::decAdjTile() {
	vFreeAdjTiles--;
}

void gameTile::setAdjTile(int num) {
	vFreeAdjTiles = num;
}
