#include "block.h"

Block::Block(string name, int width, int height)
{
    mName = name;
    mX = 0;
    mY = 0;
    mWidth = width;
    mHeight = height;
    mRotate = false;
}

string Block::GetName()
{
    return mName;
}

int Block::GetX()
{
    return mX;
}

int Block::GetY()
{
    return mY;
}

void Block::SetX(int x)
{
    mX = x;
}

void Block::SetY(int y)
{
    mY = y;
}

int Block::GetWidth()
{
    return mRotate ? mHeight : mWidth;
}

int Block::GetHeight()
{
    return mRotate ? mWidth : mHeight;
}

void Block::Rotate()
{
    if (mRotate == true)
    {
        mRotate = false;
    }
    else
    {
        mRotate = true;
    }
}

bool Block::GetRotate()
{
    return mRotate;
};
