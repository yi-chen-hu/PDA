#ifndef SP_H
#define SP_H

#include "block.h"
#include <fstream>
#include <vector>
using namespace std;

class SequencePair
{
public:
    SequencePair() {};
    void ReadFile(ifstream &inputFile);
    void WriteFile(ofstream &outputFile);
    void FindLocalOptimum();
    void GenerateLegalSolution();
    int EvaluateSequence(bool mode); // 0: compute x coordinates; 1: compute y coordinates
    int DetermineArea();

private:
    double mLowerBound;
    double mUpperBound;
    int mNumOfBlocks;
    vector<Block*> mBlockVector;
    bool mLegal;
    vector<int> mX, mY;
    int mChipWidth, mChipHeight;
};

#endif
