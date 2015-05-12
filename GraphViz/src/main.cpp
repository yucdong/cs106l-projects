#include <iostream>
#include "SimpleGraph.h"
#include <ctime>

using namespace std;

void Welcome();

// Main method
int main() {
    Welcome();
    return 0;
}

/* Prints a message to the console welcoming the user and
 * describing the program. */
void Welcome() {
    cout << "Welcome to CS106L GraphViz!" << endl;
    cout << "This program uses a force-directed graph layout algorithm" << endl;
    cout << "to render sleek, snazzy pictures of various graphs." << endl;
    cout << endl;
    string filename;
    double duration;
    SimpleGraph graph;

    cout << "Please enter the filename to visualize." << endl;
    getline(cin, filename);

    cout << "Please enter the Algorithm rumtime duration." << endl;
    cin >> duration;

    ReadGraph(filename.c_str(), graph);
    InitNodes(graph);
    DrawGraph(graph);


    // Iterate while time has not elapsed
    time_t startTime = time(NULL);
    while (difftime(time(NULL), startTime) < duration) {
        UpdateGraph(graph);
        DrawGraph(graph);
    }


}
