#include <string>
#include <vector>
#include <list>
#include <set>
#include <iterator>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <ctime>

using namespace std;

class Net
{
public:
    Net(string name)
    {
        this->name = name;
        this->numOfCells = 0;
        this->numOfCellsInGroup[0] = 0;
        this->numOfCellsInGroup[1] = 0;
    };
    string name;
    int numOfCells;
    int numOfCellsInGroup[2];
    vector<string> cells;
};

class Cell
{
public:
    Cell(string name)
    {
        this->name = name;
        this->group = 0; // 0: Group1; 1: Gruop2
        this->gain = 0;
        this->lock = false;
    };
    string name;
    bool group;
    int gain;
    bool lock;
    vector<string> nets;
    list<Cell*>::iterator iterToCellInBucketList;

};

class BucketList
{
public:
    BucketList(int Pmax)
    {
        this->numOfCells = 0;
        this->maxIndex = 0;
        this->linkedList.resize(2 * Pmax + 1);
    };
    int numOfCells;
    int maxIndex;
    vector<list<Cell*>> linkedList;
    set<string> cellsOfInitialPartition;
};

void initialPartition(BucketList* bucketList, vector<string>& allCells, unordered_map<string, Cell*>& cellNameToObjectCell, double lowerBound, double upperBound)
{
    bucketList[0].numOfCells = 0;
    bucketList[1].numOfCells = 0;
    int numOfCells = allCells.size();
    //----------------------------------------------------------------------------------------------------//
    //  Assign all cells to group1 or group2 arbitrarily and count the cells in 2 bucketList list seperately  //
    //----------------------------------------------------------------------------------------------------//
    for (int i = 0; i < numOfCells; i++)
    {
        if (i < numOfCells / 2)
        {
            cellNameToObjectCell[allCells[i]]->group = 0;
            bucketList[0].numOfCells++;
        }
        else
        {
            cellNameToObjectCell[allCells[i]]->group = 1;
            bucketList[1].numOfCells++;
        }
    }
    return;
}

void initialGainComputationAndBucketsSetUp(unordered_map<string, Cell*>& cellNameToObjectCell, BucketList* bucketList, vector<Net>& allNets, int Pmax, int& cutSize)
{
    for (auto& net : allNets)
    {
        //-----------------------------------------------------------------------------------//
        //                       Count the cells in 2 bucketList list seperately                 //
        //-----------------------------------------------------------------------------------//
        string cellInG1, cellInG2;
        for (auto& cellName : net.cells)
        {
            if (cellNameToObjectCell[cellName]->group == 0)
            {
                cellInG1 = cellName;
                net.numOfCellsInGroup[0]++;
            }
            else
            {
                cellInG2 = cellName;
                net.numOfCellsInGroup[1]++;
            }
        }
        //-----------------------------------------------------------------------------------//
        //                                 Calculate cutSize                                 //
        //-----------------------------------------------------------------------------------//
        if (net.numOfCellsInGroup[0] > 0 && net.numOfCellsInGroup[1] > 0)
        {
            cutSize++;
        }
        //-----------------------------------------------------------------------------------//
        //                                         FS                                        //
        //-----------------------------------------------------------------------------------//
        if (net.numOfCellsInGroup[0] == 1)
        {
            cellNameToObjectCell[cellInG1]->gain++;
        }
        if (net.numOfCellsInGroup[1] == 1)
        {
            cellNameToObjectCell[cellInG2]->gain++;
        }
        //-----------------------------------------------------------------------------------//
        //                                         TE                                        //
        //-----------------------------------------------------------------------------------//
        if (net.numOfCellsInGroup[0] == net.cells.size())
        {
            for (auto& cellName : net.cells)
            {
                cellNameToObjectCell[cellName]->gain--;
            }
        }
        if (net.numOfCellsInGroup[1] == net.cells.size())
        {
            for (auto& cellName : net.cells)
            {
                cellNameToObjectCell[cellName]->gain--;
            }
        }
    }

    for (auto& cellMap : cellNameToObjectCell)
    {
        // gain is in [-Pmax, +Pmax].
        // However, the index of linkedList which is an array is start from 0.
        // Therefore, there is an offset Pmax, so that gain could be shifted to [0, 2 * Pmax] and size of linkedList = 2 * Pmax + 1.
        int idxOfBucketList = cellMap.second->gain + Pmax;
        //-------------------------------------------------------------------------------------------------//
        //  Build up the bucketList list and save the pointer which points to the node of cell in bucketList list  //
        //-------------------------------------------------------------------------------------------------//
        if (cellMap.second->group == 0)
        {
            if (idxOfBucketList > bucketList[0].maxIndex)
            {
                bucketList[0].maxIndex = idxOfBucketList;
            }
            bucketList[0].linkedList[idxOfBucketList].push_front(cellMap.second);
            cellMap.second->iterToCellInBucketList = bucketList[0].linkedList[idxOfBucketList].begin();
            //-----------------------------------------------------------------------------------------------------------//
            //  build up cellsOfInitialPartition which would be used in function restoreBestPartition() to restore the best partition  //
            //-----------------------------------------------------------------------------------------------------------//
            bucketList[0].cellsOfInitialPartition.insert(cellMap.second->name);
        }
        else
        {
            if (idxOfBucketList > bucketList[1].maxIndex)
            {
                bucketList[1].maxIndex = idxOfBucketList;
            }
            bucketList[1].linkedList[idxOfBucketList].push_front(cellMap.second);
            cellMap.second->iterToCellInBucketList = bucketList[1].linkedList[idxOfBucketList].begin();
            bucketList[1].cellsOfInitialPartition.insert(cellMap.second->name);
        }
    }
    return;
}

void resetCellAndPartition(BucketList* bucketList, unordered_map<string, Cell*>& cellNameToObjectCell, vector<Net>& allNets)
{
    //-----------------------------------------------------------------------------------//
    //                               Re-assign group of cells                            //
    //-----------------------------------------------------------------------------------//
    for (auto& cellName : bucketList[0].cellsOfInitialPartition)
    {
        cellNameToObjectCell[cellName]->group = 0;
    }
    for (auto& cellName : bucketList[1].cellsOfInitialPartition)
    {
        cellNameToObjectCell[cellName]->group = 1;
    }
    bucketList[0].numOfCells = bucketList[0].cellsOfInitialPartition.size();
    bucketList[1].numOfCells = bucketList[1].cellsOfInitialPartition.size();
    //-----------------------------------------------------------------------------------//
    //     Clear cellsOfInitialPartition which would be calculated in function updateBucketList()      //
    //-----------------------------------------------------------------------------------//
    bucketList[0].cellsOfInitialPartition.clear();
    bucketList[1].cellsOfInitialPartition.clear();
    //----------------------------------------------------------------------------------------------------------//
    //  Reset numOfCellsInGroup which would be calculated in function initialGainComputationAndBucketsSetUp()  //
    //----------------------------------------------------------------------------------------------------------//
    for (auto& net : allNets)
    {
        net.numOfCellsInGroup[0] = 0;
        net.numOfCellsInGroup[1] = 0;
    }
    //-----------------------------------------------------------------------------------------------------------------//
    //  unlock all cells and reset gain which would be calculated in function initialGainComputationAndBucketsSetUp()  //
    //-----------------------------------------------------------------------------------------------------------------//
    for (auto& cell : cellNameToObjectCell)
    {
        cell.second->gain = 0;
        cell.second->lock = false;
    }
}

Cell* selectCell(double lowerBound, double upperBound, BucketList* bucketList)
{
    bool empty[2] = { false }; // empty[0] means the linkedList[0] is empty or not; empty[1] means the linkedList[1] is empty or not.
    int index[2] = { bucketList[0].maxIndex, bucketList[1].maxIndex }; // Initialize index[0] and index[1]. 
    Cell* targetCell = NULL;
    
    //-----------------------------------------------------------------------------------//
    //  index[0] and index[1] would decreased until the double linked list is not empty  //
    //-----------------------------------------------------------------------------------//
    while (bucketList[0].linkedList[index[0]].empty())
    {
        if (index[0] == 0)
        {
            empty[0] = true;
            break;
        }
        index[0]--;
    }
    while (bucketList[1].linkedList[index[1]].empty())
    {
        if (index[1] == 0)
        {
            empty[1] = true;
            break;
        }
        index[1]--;
    }
    
    //-----------------------------------------------------------------------------------//
    //            Start to determine which cell gonna be moved to another side           //
    //-----------------------------------------------------------------------------------//
    if (empty[0] && empty[1])
    {
        return NULL;
    }
    else if (empty[0])
    {
        targetCell = bucketList[1].linkedList[index[1]].front();
        bucketList[1].linkedList[index[1]].pop_front();
        return targetCell;
    }
    else if (empty[1])
    {
        targetCell = bucketList[0].linkedList[index[0]].front();
        bucketList[0].linkedList[index[0]].pop_front();
        return targetCell;
    }
    else
    {
        if (index[0] > index[1])
        {
            if (lowerBound <= bucketList[0].numOfCells - 1 && bucketList[0].numOfCells - 1 <= upperBound && lowerBound <= bucketList[1].numOfCells + 1 && bucketList[1].numOfCells + 1 <= upperBound)
            {
                targetCell = bucketList[0].linkedList[index[0]].front();
                bucketList[0].linkedList[index[0]].pop_front();
                return targetCell;
            }
            else
            {
                targetCell = bucketList[1].linkedList[index[1]].front();
                bucketList[1].linkedList[index[1]].pop_front();
                return targetCell;
            }
        }
        else if (index[0] == index[1])
        {
            if (lowerBound <= bucketList[0].numOfCells - 1 && bucketList[0].numOfCells - 1 <= upperBound && lowerBound <= bucketList[1].numOfCells + 1 && bucketList[1].numOfCells + 1 <= upperBound)
            {
                targetCell = bucketList[0].linkedList[index[0]].front();
                bucketList[0].linkedList[index[0]].pop_front();
                return targetCell;
            }
            else
            {
                targetCell = bucketList[1].linkedList[index[1]].front();
                bucketList[1].linkedList[index[1]].pop_front();
                return targetCell;
            }
        }
        else
        {
            if (lowerBound <= bucketList[0].numOfCells + 1 && bucketList[0].numOfCells + 1 <= upperBound && lowerBound <= bucketList[1].numOfCells - 1 && bucketList[1].numOfCells - 1 <= upperBound)
            {
                targetCell = bucketList[1].linkedList[index[1]].front();
                bucketList[1].linkedList[index[1]].pop_front();
                return targetCell;
            }
            else
            {
                targetCell = bucketList[0].linkedList[index[0]].front();
                bucketList[0].linkedList[index[0]].pop_front();
                return targetCell;
            }
        }
    }
}

void updateGroup(BucketList* bucketList, Cell* targetCell, int& cutSize)
{
    //-----------------------------------------------------------------------------------------------------------------------------------//
    //  Update the group of the cell being moved, lock this cell, update cutSize and update number of cells in 2 bucketList list seperately  //
    //-----------------------------------------------------------------------------------------------------------------------------------//
    bool from = targetCell->group;
    bool to = !from;
    targetCell->group = to;
    targetCell->lock = true;
    cutSize -= targetCell->gain;
    bucketList[from].numOfCells--;
    bucketList[to].numOfCells++;
    return;
}

void updateBucketList(BucketList* bucketList, Cell* cell, int Pmax, int delta)
{
    int i = cell->gain + Pmax;
    //-----------------------------------------------------------------------------------//
    //             Move the cell from one linked list to another linked list             //
    //-----------------------------------------------------------------------------------//
    bucketList[cell->group].linkedList[i].erase(cell->iterToCellInBucketList);
    bucketList[cell->group].linkedList[i + delta].push_front(cell);
    cell->iterToCellInBucketList = bucketList[cell->group].linkedList[i + delta].begin();
    //-----------------------------------------------------------------------------------//
    //            Start to determine which cell gonna be moved to another side           //
    //-----------------------------------------------------------------------------------//
    if (i + delta > bucketList[cell->group].maxIndex)
    {
        bucketList[cell->group].maxIndex = i + delta;
    }
    return;
}

Cell* getCriticalCell(Net& net, unordered_map<string, Cell*>& cellNameToObjectCell, Cell* targetCell, bool group)
{
    for (auto& cellName : net.cells)
    {
        if (cellNameToObjectCell[cellName]->group == group)
        {
            if (cellNameToObjectCell[cellName] != targetCell)
            {
                return cellNameToObjectCell[cellName];
            }
        }
    }
    return NULL;
}

void updateGain(BucketList* bucketList, unordered_map<string, Cell*>& cellNameToObjectCell, Cell* targetCell, Net& net, int Pmax, bool from)
{
    bool to = !from;
    if (net.numOfCellsInGroup[to] == 0)
    {
        //-----------------------------------------------------------------------------------//
        //              T(n) = 0, increment gains of all free cells on this net              //
        //-----------------------------------------------------------------------------------//
        for (auto& cellName : net.cells)
        {
            if (cellNameToObjectCell[cellName]->lock == false)
            {
                updateBucketList(bucketList, cellNameToObjectCell[cellName], Pmax, 1);
                cellNameToObjectCell[cellName]->gain++;
            }
        }
    }
    else if (net.numOfCellsInGroup[to] == 1)
    {
        //-----------------------------------------------------------------------------------//
        //        T(n) = 1, decrement gain of the only cell on "to" side if it is free       //
        //-----------------------------------------------------------------------------------//
        Cell* criticalCell = getCriticalCell(net, cellNameToObjectCell, targetCell, to);
        if (criticalCell->lock == false)
        {
            updateBucketList(bucketList, criticalCell, Pmax, -1);
            criticalCell->gain--;
        }
    }
    //-----------------------------------------------------------------------------------//
    //                Update the number of cells of 2 bucketList list seperately             //
    //-----------------------------------------------------------------------------------//
    net.numOfCellsInGroup[from]--;
    net.numOfCellsInGroup[to]++;
    
    if (net.numOfCellsInGroup[from] == 0)
    {
        //-----------------------------------------------------------------------------------//
        //             F(n) = 0, decrement gains of all free cells on this net               //
        //-----------------------------------------------------------------------------------//
        for (auto& cellName : net.cells)
        {
            if (cellNameToObjectCell[cellName]->lock == false)
            {
                updateBucketList(bucketList, cellNameToObjectCell[cellName], Pmax, -1);
                cellNameToObjectCell[cellName]->gain--;
            }
        }
    }
    else if (net.numOfCellsInGroup[from] == 1)
    {
        //-----------------------------------------------------------------------------------//
        //      F(n) = 1, increment gain of the only cell on "from" side if it is free       //
        //-----------------------------------------------------------------------------------//
        Cell* criticalCell = getCriticalCell(net, cellNameToObjectCell, targetCell, from);
        if (criticalCell->lock == false)
        {
            updateBucketList(bucketList, criticalCell, Pmax, 1);
            criticalCell->gain++;
        }
    }
    return;
}

void restoreBestPartition(BucketList* bucketList, vector<pair<string, bool>>& move, int bestRound)
{
    //-----------------------------------------------------------------------------------//
    //      cellsOfInitialPartition is initial partition and use it to restore the best partition      //
    //-----------------------------------------------------------------------------------//
    for (int i = 0; i < bestRound; i++)
    {
        bool from = move[i].second;
        bool to = !from;
        string cellName = move[i].first;
        bucketList[from].cellsOfInitialPartition.erase(cellName);
        bucketList[to].cellsOfInitialPartition.insert(cellName);
    }
    return;
}

int main(int argc, char** argv)
{
    double startTime = clock();
    ifstream inputFile(argv[1]);
    ofstream outputFile(argv[2]);
    int Pmax = 0;
    unordered_map<string, Cell*> cellNameToObjectCell; // Map cell name to object cell.
    unordered_map<string, int> netNameToNetIndex; // The input of net is discontinuous, so use a map to store the index of net.
    vector<string> allCells;
    vector<Net> allNets;
    double r;
    //-----------------------------------------------------------------------------------//
    //           Start to read file and build up the data structure which we need        //
    //-----------------------------------------------------------------------------------//
    cout << "Start to read file" << endl;
    inputFile >> r;
    string identifier;
    while (inputFile >> identifier)
    {
        if (identifier == "NET") // Look for "NET" first.
        {
            string readText;
            inputFile >> readText; // Read net name.
            allNets.push_back(Net(readText));
            netNameToNetIndex[readText] = allNets.size() - 1;
            unordered_map<string, bool> cellMap; // In order to know whether the cell has been saved or not, build a map to save this info.
            cellMap.clear(); // Clear the map first.
            while (inputFile >> readText)
            {
                if (readText == ";") // When encountering semicolon, break the while loop to avoid saving cells
                {
                    break;
                }
                else // Save cells.
                {
                    if (cellNameToObjectCell.find(readText) == cellNameToObjectCell.end()) // Determine whether the cell has been saved or not.
                    {
                        Cell* cell = new Cell(readText);
                        cellNameToObjectCell[cell->name] = cell;
                        allCells.push_back(readText);
                    }
                    if (cellMap.find(readText) == cellMap.end()) // Determine whether the cell has been saved or not.
                    {
                        allNets.back().cells.push_back(readText);
                        allNets.back().numOfCells++;
                        cellNameToObjectCell[readText]->nets.push_back(allNets.back().name);
                        cellMap[readText] = true;
                    }
                    int p = cellNameToObjectCell[readText]->nets.size();
                    if (p > Pmax)
                    {
                        Pmax = p;
                    }
                }
            }
        }
    }
    cout << "End of reading file" << endl;
    inputFile.close();
    double lowerBound = double((double)(allCells.size() * 0.5) - (int)(r * allCells.size() * 0.5));
    double upperBound = (1 + r) * allCells.size() / 2.0;
    int cutSize = 0;
    BucketList bucketList[2] = { BucketList(Pmax), BucketList(Pmax) };
    //-----------------------------------------------------------------------------------//
    //                          Start to implement FM algorithm                          //
    //-----------------------------------------------------------------------------------//
    initialPartition(bucketList, allCells, cellNameToObjectCell, lowerBound, upperBound);
    initialGainComputationAndBucketsSetUp(cellNameToObjectCell, bucketList, allNets, Pmax, cutSize); // Determine the cut size and build the bucketList list.
    bool firstTime = true;
    int minCutSize = 0;
    int iteration = 0;
    int Gm = 0;
    do
    {
        Gm = 0;
        int round = 0;
        int bestRound = 0;
        int tempGm = 0;
        vector<pair<string, bool>> move; // Save every move and then use it to restore the best partition.
        iteration++;
        cout << "iteration = " << iteration << endl;
        if (firstTime == false)
        {
            cutSize = 0;
            resetCellAndPartition(bucketList, cellNameToObjectCell, allNets); // Free all cells and restore the best partition which is gotten from last iteration.
            initialGainComputationAndBucketsSetUp(cellNameToObjectCell, bucketList, allNets, Pmax, cutSize); // Determine cut size and re-build the bucketList list.
        }
        minCutSize = cutSize;
        Cell* targetCell = selectCell(lowerBound, upperBound, bucketList);
        while (targetCell != NULL) // Keep moving cell until there is no cell which has not been moved.
        {
            round++;
            tempGm += targetCell->gain;
            bool from = targetCell->group;
            move.push_back(make_pair(targetCell->name, from));
            updateGroup(bucketList, targetCell, cutSize);
            if (cutSize < minCutSize)
            {
                minCutSize = cutSize;
                Gm = tempGm;
                bestRound = round;
            }
            for (auto& netString : targetCell->nets)
            {
                updateGain(bucketList, cellNameToObjectCell, targetCell, allNets[netNameToNetIndex[netString]], Pmax, from);
            }
            targetCell = selectCell(lowerBound, upperBound, bucketList);
        }
        // end of moving cells
        restoreBestPartition(bucketList, move, bestRound);
        firstTime = false;
        cout << "Gm = " << Gm << endl;
        double endTime = clock();
        double totalTime = (endTime - startTime) / CLOCKS_PER_SEC;
        if (iteration > 1 && totalTime > 15) // 15 seconds. Avoid to violate runtime limit and make sure running algorithm at least one time.
        {
            break;
        }
    } while (Gm > 0);
    //-----------------------------------------------------------------------------------//
    //                         Write the result into output file                         //
    //-----------------------------------------------------------------------------------//
    outputFile << "Cutsize = " << minCutSize << endl;
    outputFile << "G1 " << bucketList[0].cellsOfInitialPartition.size() << endl;
    for (auto cellName : bucketList[0].cellsOfInitialPartition)
    {
        outputFile << cellName << " ";
    }
    outputFile << ";" << endl;
    outputFile << "G2 " << bucketList[1].cellsOfInitialPartition.size() << endl;
    for (auto cellName : bucketList[1].cellsOfInitialPartition)
    {
        outputFile << cellName << " ";
    }
    outputFile << ";" << endl;
    outputFile.close();
    return 0;
}