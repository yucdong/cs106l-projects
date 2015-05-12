#ifndef UTILS_H
#define UTILS_H
#include "Game.h"

/* Read the dictionary and form the game structure */
void ReadDict(Game& game);

int GetInteger(int &length);

int GetLetter(char &letter);

void HangmanLogic(Game& game);

#endif // UTILS_H
