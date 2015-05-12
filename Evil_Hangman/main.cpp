#include <iostream>
#include "Utils.h"

using namespace std;

int main()
{

    /* Read words from dictionary */
    Game game;

    ReadDict(game);

    while (true) {

        // Prompt the user to enter the word's length
        cout << "Please enter a word length." << endl;
        int res = GetInteger(game.length);
        if (!res) {
            cout << "See you next time!" << endl;
            break;
        }

        if (!game.validLength()) {
            cout << "No word has length: " << game.length << endl;
            continue;
        } else {

            // Prompt the user to enter the number of guesses
            while (true) {
                cout << "Please enter number of guesses" << endl;
                res = GetInteger(game.numGuess);
                if (!res || game.numGuess <= 0) {
                    cout << "Invalid number of guess" << endl;
                } else
                    break;

                cout << "Retry: " << endl;
            }

            // Prompt the user for whether he want a running total of the words remaining in the list
            cout << "Do you want to show the remaining words in the list?Y/N:";
            char runningtotal;
            GetLetter(runningtotal);
            if (runningtotal == 'Y' || runningtotal == 'y')
                game.showRemainWords = true;
            else
                game.showRemainWords = false;


            HangmanLogic(game);

        }


        cout << "Would you like to play the game again? [Y/N]";
        char again;
        cin >> again;
        if (again == 'N') break;

    }

    return 0;
}

