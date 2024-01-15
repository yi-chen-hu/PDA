#include "sp.h"
#include "veb.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>
#include <string>

using namespace std;

void SequencePair::ReadFile(ifstream &inputFile)
{
    string name;
    int width;
    int height;

    inputFile >> mLowerBound >> mUpperBound;
    while (inputFile >> name >> width >> height)
    {
        Block* block = new Block(name, width, height);
        mBlockVector.push_back(block);
    }
    mNumOfBlocks = mBlockVector.size();
}

void SequencePair::GenerateLegalSolution()
{
    int chipWidth;
    int chipHeight;
    mLegal = false;
    //===========================================================================//
    //                         Initialize sequence pair                          //
    //===========================================================================//
    for (int i = 0; i < mNumOfBlocks; i++)
    {
        mX.push_back(i);
        mY.push_back(i);
    }
    //===========================================================================//
    //          Shuffle the sequence pair to look for a legal solution           //
    //===========================================================================//
    while (mLegal == false)
    {
        random_shuffle(mX.begin(), mX.end());
        random_shuffle(mY.begin(), mY.end());
        chipWidth = this->EvaluateSequence(0);
        chipHeight = this->EvaluateSequence(1);
        if (mLowerBound <= (double) chipWidth / chipHeight && (double) chipWidth / chipHeight <= mUpperBound)
        {
            mLegal = true;
        }
    }
}

void SequencePair::FindLocalOptimum()
{
    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<int> randOp(0, 2);
    uniform_int_distribution<int> randBlock(0, mNumOfBlocks - 1);

    int round = 0;
    int perturbationPerRound = 10;
    int prevChipWidth = mChipWidth;
    int prevChipHeight = mChipHeight;
    int repeatTime = 0;
    while (1)
    {
        for (int i = 0; i < perturbationPerRound; i++)
        {
            //==============================================================================//
            //                            Perturbate the sequence pair                      //
            //==============================================================================//
            int op = randOp(mt);
            int a = randBlock(mt);
            int b = randBlock(mt);
            if (op == 0)
            {
                int temp = mX[a];
                mX[a] = mX[b];
                mX[b] = temp;
                
            }
            else if (op == 1)
            {
                int temp = mY[a];
                mY[a] = mY[b];
                mY[b] = temp;
            }
            else if (op == 2)
            {
                mBlockVector[a]->Rotate();
            }
            
            //==============================================================================//
            //                      Determine longest path of sequence pair                 //
            //==============================================================================//
            int chipWidth = this->EvaluateSequence(0);
            int chipHeight = this->EvaluateSequence(1);
            
            if (mLowerBound <= (double) chipWidth / chipHeight && (double) chipWidth / chipHeight <= mUpperBound)
            {
                mLegal = true;
            }
            else
            {
                mLegal = false;
            }
            
            bool smallerArea;
            if (chipWidth * chipHeight <= mChipWidth * mChipHeight)
            {
                smallerArea = true;
            }
            else
            {
                smallerArea = false;
            }
            
            //==============================================================================//
            //                Undo the operation if it is not a better solution             //
            //==============================================================================//
            if (smallerArea == false || mLegal == false)
            {
                if (op == 0)
                {
                    int temp = mX[a];
                    mX[a] = mX[b];
                    mX[b] = temp;
                }
                else if (op == 1)
                {
                    int temp = mY[a];
                    mY[a] = mY[b];
                    mY[b] = temp;
                }
                else if (op == 2)
                {
                    mBlockVector[a]->Rotate();
                }
            }
        }
        //==============================================================================//
        //                   Determine the chip area after 10 perturbation              //
        //==============================================================================//
        mChipWidth = this->EvaluateSequence(0);
        mChipHeight = this->EvaluateSequence(1);
        if (mLowerBound <= (double) mChipWidth / mChipHeight && (double) mChipWidth / mChipHeight <= mUpperBound)
        {
            mLegal = true;
        }
        else
        {
            mLegal = false;
        }
        
        //==============================================================================//
        //             If the chip area doesn't decrease, break the while loop          //
        //==============================================================================//
        if (round % 100 == 0)
        {
            if (prevChipWidth == mChipWidth && prevChipHeight == mChipHeight)
            {
                repeatTime++;
            }
            else
            { 
                prevChipWidth = mChipWidth;
                prevChipHeight = mChipHeight;
                repeatTime = 0;
            }
            
            // cout << "Perturbation Time = " << round * 10 << ", chip area = " << this->DetermineArea() << endl;
        }
        round++;
        
        if (repeatTime > 1 || round > 5000)
        {
            break;
        }
    }
}

//==============================================================================//
//                      Find the longest path of sequence pair                  //
//==============================================================================//
int SequencePair::EvaluateSequence(bool mode)
{
    if (mode == 1)
    {
        reverse(mX.begin(), mX.end());
    }
    vector<int> matchY;
    matchY.resize(mNumOfBlocks);
    for (int i = 0; i < mNumOfBlocks; ++i)
    {
        matchY[mY[i]] = i;
    }
    VanEmdeBoas* H = new VanEmdeBoas(mNumOfBlocks + 1);
    H->Insert(0);
    vector<int> BUCKL(mNumOfBlocks + 1, 0);
    for (int i = 0; i < mNumOfBlocks; ++i)
    {
        int b = mX[i];
        int p = matchY[b];
        H->Insert(p);
        if (mode == 0)
        {
            int pos = BUCKL[H->Predecessor(p)];
            BUCKL[p] = pos + mBlockVector[b]->GetWidth();
            mBlockVector[b]->SetX(pos);
        }
        else
        {
            int pos = BUCKL[H->Predecessor(p)];
            BUCKL[p] = pos + mBlockVector[b]->GetHeight();
            mBlockVector[b]->SetY(pos);
        }
        int k = p;
        while (H->Successor(k) != -1)
        {
            k = H->Successor(k);
            if (BUCKL[k] <= BUCKL[p])
            {
                H->Delete(k);
            }
        }
    }
    if (mode == 1)
    {
        reverse(mX.begin(), mX.end());
    }
    
    return BUCKL[H->GetMax()]; 
}

int SequencePair::DetermineArea()
{
    return mChipWidth * mChipHeight;
}

void SequencePair::WriteFile(ofstream &outputFile)
{
    mChipWidth = this->EvaluateSequence(0);
    mChipHeight = this->EvaluateSequence(1);
    
    outputFile << "A = " << this->DetermineArea() << endl;    
    outputFile << "R = " << (double) mChipWidth / mChipHeight << endl;
    for (int i = 0; i < mNumOfBlocks; i++)
    {
        Block* block = mBlockVector[i];
        outputFile << block->GetName() << " " << block->GetX() << " " << block->GetY();
        if (block->GetRotate())
        {
            outputFile << " R" << endl;
        }
        else
        {
            outputFile << endl;
        }
    }
}
