#include "game_logic.h"
#include <iostream>

/* The first block of code contains the getter functions for the mine
** sweeper's board settings. These all take no input and returns an
** integer value for its respective member variable.
*/
int boardOptions::getWidth() {
    return width;
}

int boardOptions::getHeight() {
    return height;
}

int boardOptions::getMines() {
    return mines;
}

bool boardOptions::setOpt(int w, int h, int m) {
    int max = (w - 1) * (h - 1);
    int min = (w * h) / 10;
    if (w < 9 || h < 9 || m < min || m > max) {
        return 0;
    }
    width = w;
    height = h;
    mines = m;
    return 1;
}