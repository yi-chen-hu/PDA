#include "sp.h"
#include "veb.h"
#include <fstream>
#include <iostream>
#include <iomanip>
using namespace std;

int main(int argc, char* argv[])
{
    ifstream inputFile;
    ofstream outputFile;
    inputFile.open(argv[1]);
    outputFile.open(argv[2]);
    
    SequencePair sp;
    sp.ReadFile(inputFile);
    // Initialize a legal solution first.
    sp.GenerateLegalSolution();
    // Perturbate the blocks and compare the chip area with previous one.
    // My goal is small runtime rather than small chip area, so I don't implement simulated annealing.
    sp.FindLocalOptimum();
    sp.WriteFile(outputFile);
    inputFile.close();
    outputFile.close();

    return 0;
}
