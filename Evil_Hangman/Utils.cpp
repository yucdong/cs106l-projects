#include "Utils.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cstdlib>
#include <set>
#include <map>
#include <cctype>
#include <algorithm>

using namespace std;

static string genWordHint(string &chosenword, set<char> &guessedLetters) {
    stringstream builder;
    for (char c : chosenword) {
        if (guessedLetters.find(c) != guessedLetters.end())
            builder << c;
        else
            builder << '_';
    }

    string result;
    builder >> result;
    return result;
}

static int RecalculateRemainWords(vector<string> &words, const char letter, size_t length) {
    map<string, vector<string> > patternToWords;
    string frequentPattern(length, '-');
    int frequentCount = 0;

    for (string word : words) {
        string pattern(length, '-');
        for (size_t pos = 0; pos < length; ++pos) {
            if (word[pos] == letter)
                pattern[pos] = letter;

        }

        patternToWords[pattern].push_back(word);
        if (patternToWords[pattern].size() > (size_t)frequentCount) {
            frequentCount = patternToWords[pattern].size();
            frequentPattern = pattern;
        }
    }

    words.assign(patternToWords[frequentPattern].begin(), patternToWords[frequentPattern].end());
    return words.size();
}

void HangmanLogic(Game &game) {
    cout << setfill('-') << setw(10) << "" << endl;
    cout << setfill(' ') << setw(10) << "GAME START" << endl;
    cout << setfill('-') << setw(10) << "" << setfill(' ') << endl;
    cout << endl;

    vector<string> remainWords = game.lenToWords[game.length];
    set<char> guessedChars;

    while (game.numGuess > 0) {
        cout << "You have " << game.numGuess << " guesses remaining." << endl;
        cout << "Word hint:" << genWordHint(remainWords[0], guessedChars) << endl;
        cout << "There are " << remainWords.size() << " eligible words left." << endl;
        cout << endl;

        char letter;
        cout << "Guess a letter:";
        while (true) {
            if (!GetLetter(letter)) {
                cout << "Guess a letter:";
                continue;
            }

            if (guessedChars.find(letter) != guessedChars.end()) {
                cout << "You tried this letter before, choose another one!" << endl;
                cout << "Guess a letter:";
                continue;
            } else {
                break;
            }
        }

        game.numGuess--;
        guessedChars.insert(letter);
        if (1 == RecalculateRemainWords(remainWords, letter, game.length)) {
            cout << "Congratulations, You have guessed the word: " << genWordHint(remainWords[0], guessedChars) << endl;
            break;
        }

    }

    if (remainWords.size() > 1)
        cout << "Sorry, You failed.\n" << endl;
}

void ReadDict(Game &game) {

    // Open dictionary
    ifstream dict("/home/tavares/cs106l/Evil_Hangman/Evil_Hangman/Evil_Hangman/dictionary.txt");
    if (!dict.is_open()) {
        cerr << "Cannot find 'dictionary.txt'" << endl;
        return;
    }

    game.lens.clear();
    game.lenToWords.clear();

    string word;
    while (getline(dict, word)) {
        size_t len = word.size();
        game.lens.insert(len);
        game.lenToWords[len].push_back(word);
    }


}

static string GetLine() {
    string result;
    getline(cin, result);
    return result;
}

int GetInteger(int &length) {
    while (true) {
        stringstream converter;
        converter << GetLine();

        int result;
        if (converter >> result) {
            char remaining;
            if (converter >> remaining)
                cout << "Unexpected character: " << remaining << endl;
            else {
                length = result;
                return 1;
            }
        } else
            cout << "Please enter an integer." << endl;
        cout << "Retry: ";
    }
}


int GetLetter(char &letter) {
    while (true) {
        stringstream converter;
        converter << GetLine();

        if (converter >> letter) {
            char remaining;
            if (converter >> remaining)
                cout << "Unexpected character: " << remaining << endl;
            else {
                if (!isalpha(letter)) {
                    cout << "Please enter an alphabetic letter" << endl;

                } else {
                    letter = tolower(letter);
                    return 1;
                }
            }
        } else {
            cout << "Please enter a character." << endl;

        }

        cout << "Retry: ";
    }
}




















