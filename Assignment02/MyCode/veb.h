#ifndef VEB_H
#define VEB_H

#include <cmath>
#include <vector>
using namespace std;

class VanEmdeBoas
{
public: 
    VanEmdeBoas(int u);
    int GetMin();
    int GetMax();
    void SetMin(int value);
    void SetMax(int value);
    int GenerateIndex(int x, int y);
    void Insert(int key);
    void Delete(int key);
    int Successor(int key);
    int Predecessor(int key);

private:
    int universeSize;
    int min;
    int max;
    VanEmdeBoas* summary;
    vector<VanEmdeBoas*> clusters;
};

#endif
