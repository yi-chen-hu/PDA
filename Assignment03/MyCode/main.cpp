#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <unordered_set>
#include <iomanip>

using namespace std;

class Mos
{
public:
    Mos(string name, string drain, string gate, string source, string type, double width, double length)
    {
        this->mName = name;
        this->mDrain = drain;
        this->mGate = gate;
        this->mSource = source;
        this->mType = type;
        this->mWidth = width;
        this->mLength = length;
    };

    string mName;
    string mDrain;
    string mGate;
    string mSource;
    string mType;
    double mWidth;
    double mLength;
};


bool compareMosByGate(Mos* mos1, Mos* mos2)
{
    return mos1->mGate < mos2->mGate;
}

void ReadFile(ifstream& inputFile, vector<Mos*>& NmosVector, vector<Mos*>& PmosVector, unordered_set<string>& pinSet)
{
    string text;
    getline(inputFile, text);

    while (inputFile >> text)
    {
        if (text == ".ENDS")
        {
            break;
        }
        else
        {
            string name;
            string drain;
            string gate;
            string source;
            string substrate;
            string type;
            string width;
            string length;
            string fin;

            name = text;
            inputFile >> drain >> gate >> source >> substrate >> type >> width >> length >> fin;
            double widthValue = stod(width.substr(2, width.length() - 1));
            double lengthValue = stod(length.substr(2, length.length() - 1));
            // cout << name << " " << drain << " " << gate << " " << source << " " << type << " " << widthValue << " " << lengthValue << endl;
            Mos* mos = new Mos(name, drain, gate, source, type, widthValue, lengthValue);
            pinSet.insert(drain);
            pinSet.insert(source);
            if (type == "nmos_rvt")
            {
                NmosVector.push_back(mos);
            }
            else if (type == "pmos_rvt")
            {
                PmosVector.push_back(mos);
            }
        }
    }

    sort(NmosVector.begin(), NmosVector.end(), compareMosByGate);
    sort(PmosVector.begin(), PmosVector.end(), compareMosByGate);
    
    /*
    for (int i = 0; i < PmosVector.size(); i++)
    {
        if (i < PmosVector.size() - 1)
        {
            cout << PmosVector[i]->mSource << " " << PmosVector[i]->mDrain <<  " || ";
        }
        else
        {
            cout << PmosVector[i]->mSource << " " << PmosVector[i]->mDrain << endl;
        }
    }
    
    for (int i = 0; i < NmosVector.size(); i++)
    {
        if (i < NmosVector.size() - 1)
        {
            cout << NmosVector[i]->mSource << " " << NmosVector[i]->mDrain << " || ";
        }
        else
        {
            cout << NmosVector[i]->mSource << " " << NmosVector[i]->mDrain << endl;
        }
    }
    */

}


double HPWL(vector<Mos*> NmosVector, vector<Mos*> PmosVector, unordered_set<string> pinSet)
{
    double NmosH = NmosVector[0]->mWidth;
    double PmosH = PmosVector[0]->mWidth;
    // cout << NmosH << " " << PmosH << endl;
    vector<string> NmosPinVector;
    vector<string> PmosPinVector;
    for (int i = 0; i < NmosVector.size(); i++)
    {
        NmosPinVector.push_back(NmosVector[i]->mSource);
        NmosPinVector.push_back(NmosVector[i]->mDrain);
    }
    for (auto& iter : PmosVector)
    {
        PmosPinVector.push_back(iter->mSource);
        PmosPinVector.push_back(iter->mDrain);
    }

    double result = 0;
    for (const auto& element : pinSet)
    {
        string targetString = element;
        // cout << "element = " << element << endl;

        auto leftIndexForNmos = find(NmosPinVector.begin(), NmosPinVector.end(), targetString);
        auto rightIndexForNmos = find(NmosPinVector.rbegin(), NmosPinVector.rend(), targetString);
        auto leftIndexForPmos = find(PmosPinVector.begin(), PmosPinVector.end(), targetString);
        auto rightIndexForPmos = find(PmosPinVector.rbegin(), PmosPinVector.rend(), targetString);

        int NmosLeftIndex;
        int NmosRightIndex;
        int PmosLeftIndex;
        int PmosRightIndex;
        bool existInNmos;
        bool exitstInPmos;
        if (leftIndexForNmos != NmosPinVector.end() && rightIndexForNmos != NmosPinVector.rend())
        {
            NmosLeftIndex = distance(NmosPinVector.begin(), leftIndexForNmos);
            NmosRightIndex = distance(NmosPinVector.begin(), rightIndexForNmos.base() - 1);
            existInNmos = true;
        }
        else
        {
            existInNmos = false;
        }

        if (leftIndexForPmos != PmosPinVector.end() && rightIndexForPmos != PmosPinVector.rend())
        {
            PmosLeftIndex = distance(PmosPinVector.begin(), leftIndexForPmos);
            PmosRightIndex = distance(PmosPinVector.begin(), rightIndexForPmos.base() - 1);
            exitstInPmos = true;
        }
        else
        {
            exitstInPmos = false;
        }

        int finalLeftIndex;
        int finalRightIndex;

        if (existInNmos && exitstInPmos)
        {
            if (NmosLeftIndex < PmosLeftIndex)
            {
                finalLeftIndex = NmosLeftIndex;
            }
            else
            {
                finalLeftIndex = PmosLeftIndex;
            }

            if (NmosRightIndex > PmosRightIndex)
            {
                finalRightIndex = NmosRightIndex;
            }
            else
            {
                finalRightIndex = PmosRightIndex;
            }

            result += 27.0 + NmosH / 2.0 + PmosH / 2.0;

        }
        else if (existInNmos)
        {
            finalLeftIndex = NmosLeftIndex;
            finalRightIndex = NmosRightIndex;
        }
        else if (exitstInPmos)
        {
            finalLeftIndex = PmosLeftIndex;
            finalRightIndex = PmosRightIndex;
        }

        // cout << "L = " << finalLeftIndex << endl;
        // cout << "R = " << finalRightIndex << endl;

        int indexDiff = finalRightIndex - finalLeftIndex;

        if (indexDiff != 0)
        {
            result += (finalRightIndex / 2 - finalLeftIndex / 2) * 74 + (finalRightIndex / 2 - finalLeftIndex / 2 - 1) * 88;
            
            if (finalLeftIndex % 2 == 1)
            {
                result += 17;
            }
            else
            {
                if (finalLeftIndex == 0)
                {
                    result += 66.5;
                }
                else
                {
                    result += 71;
                }
            }
            
            if (finalRightIndex % 2 == 1)
            {
                if (finalRightIndex == NmosVector.size() - 1)
                {
                    result += 66.5;
                }
                else
                {
                    result += 71;
                }
            }
            else
            {
                result += 17;
            }
        }
        
        // cout << "result = " << result << endl;
    }
    return result;
}

void WriteFile(ofstream& outputFile, double result, vector<Mos*> NmosVector, vector<Mos*> PmosVector)
{
    outputFile << result << endl;

    for (int i = 0; i < PmosVector.size(); i++)
    {
        if (i < PmosVector.size() - 1)
        {
            outputFile << PmosVector[i]->mName.substr(1) << " Dummy " ;
        }
        else
        {
            outputFile << PmosVector[i]->mName.substr(1) << endl;
        }
    }
    
    for (int i = 0; i < PmosVector.size(); i++)
    {
        if (i < PmosVector.size() - 1)
        {
            outputFile << PmosVector[i]->mSource << " " << PmosVector[i]->mGate << " " << PmosVector[i]->mDrain << " Dummy ";
        }
        else
        {
            outputFile << PmosVector[i]->mSource << " " << PmosVector[i]->mGate << " " << PmosVector[i]->mDrain << endl;
        }
    }

    for (int i = 0; i < NmosVector.size(); i++)
    {
        if (i < NmosVector.size() - 1)
        {
            outputFile << NmosVector[i]->mName.substr(1) << " Dummy " ;
        }
        else
        {
            outputFile << NmosVector[i]->mName.substr(1) << endl;
        }
    }

    for (int i = 0; i < NmosVector.size(); i++)
    {
        if (i < NmosVector.size() - 1)
        {
            outputFile << NmosVector[i]->mSource << " " << NmosVector[i]->mGate << " " << NmosVector[i]->mDrain << " Dummy ";
        }
        else
        {
            outputFile << NmosVector[i]->mSource << " " << NmosVector[i]->mGate << " " << NmosVector[i]->mDrain << endl;
        }
    }

}



int main(int argc, char* argv[])
{
    ifstream inputFile;
    ofstream outputFile;
    inputFile.open(argv[1]);
    outputFile.open(argv[2]);
    vector<Mos*> NmosVector;
    vector<Mos*> PmosVector;
    unordered_set<string> pinSet;
    ReadFile(inputFile, NmosVector, PmosVector, pinSet);
    double result = HPWL(NmosVector, PmosVector, pinSet);
    WriteFile(outputFile, result, NmosVector, PmosVector);
    
    inputFile.close();
    outputFile.close();

    return 0;
}

