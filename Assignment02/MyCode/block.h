#ifndef BLOCK_H
#define BLOCK_H

#include <string>
#include <vector>
using namespace std;

class Block
{
public:
    Block(string name, int width, int height);
    string GetName();
    int GetX();
    int GetY();
    void SetX(int x);
    void SetY(int y);
    int GetWidth();
    int GetHeight();
    void Rotate();
    bool GetRotate();

private:
    string mName;
    int mX;
    int mY;
    bool mRotate;
    int mWidth;
    int mHeight;
};

#endif
