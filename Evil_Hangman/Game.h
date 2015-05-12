#ifndef GAME_H
#define GAME_H
#include "Game.h"
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>

using namespace std;

class Game {
public:

    Game() {

    }

    bool validLength() {
        return lens.find(length) != lens.end();
    }


    int length;
    string genWordHint();
    string chosenword;

    int numGuess;
    bool showRemainWords;
    std::unordered_map<int, vector<string> > lenToWords;
    std::unordered_set<int> lens;   // lengths of all words
};

#endif // GAME_H
