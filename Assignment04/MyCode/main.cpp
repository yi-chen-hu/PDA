#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <algorithm>

using namespace std;

class DirectedGraph
{
public:
    void AddEdge(int source, int destination)
    {
        if (find(outEdges[source].begin(), outEdges[source].end(), destination) == outEdges[source].end())
        {
            outEdges[source].push_back(destination);
            inEdges[destination].push_back(source);
        }
    }

    void RemoveNode(int nodeID)
    {
        vector<int> outNodes = outEdges[nodeID];
        for (int i = 0; i < outNodes.size(); i++)
        {
            int currNodeID = outNodes[i];
            auto newEnd = remove(inEdges[currNodeID].begin(), inEdges[currNodeID].end(), nodeID);
            inEdges[currNodeID].erase(newEnd, inEdges[currNodeID].end());
        }
    }

    bool Removable(int nodeID)
    {
        if (inEdges[nodeID].size() == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

private:
    unordered_map<int, vector<int>> outEdges;
    unordered_map<int, vector<int>> inEdges;
};

int main(int argc, char* argv[])
{
    ifstream inputFile;
    ofstream outputFile;
    inputFile.open(argv[1]);
    outputFile.open(argv[2]);
    vector<string> lines;
    string line;

    while (getline(inputFile, line))
    {
        lines.push_back(line);
    }
    inputFile.close();

    stringstream topData(lines[lines.size() - 2]);
    stringstream bottomData(lines[lines.size() - 1]);
    unordered_map<int, pair<int, int>> netToInterval;
    int netName;
    vector<int> topNodeID;
    vector<int> bottomNodeID;
    int currCol = 0;
    while (topData >> netName)
    {
        topNodeID.push_back(netName);
        if (netName == 0)
        {
            currCol++;
            continue;
        }

        auto iter = netToInterval.find(netName);
        if (iter != netToInterval.end())
        {
            if (iter->second.second == -1)
            {
                iter->second.second = currCol;
            }
            else if (currCol > iter->second.second)
            {
                iter->second.second = currCol;
            }
        }
        else
        {
            netToInterval[netName] = make_pair(currCol, -1);
        }
        currCol++;
    }

    currCol = 0;
    while (bottomData >> netName)
    {
        bottomNodeID.push_back(netName);
        if (netName == 0)
        {
            currCol++;
            continue;
        }

        auto iter = netToInterval.find(netName);
        if (iter != netToInterval.end())
        {
            if (iter->second.second == -1)
            {
                if (currCol > iter->second.first)
                {
                    iter->second.second = currCol;
                }
                else
                {
                    iter->second.second = iter->second.first;
                    iter->second.first = currCol;
                }
            }
            else if (currCol > iter->second.second)
            {
                iter->second.second = currCol;
            }
            else if (currCol < iter->second.first)
            {
                iter->second.first = currCol;
            }
        }
        else
        {
            netToInterval[netName] = make_pair(currCol, -1);
        }
        currCol++;
    }

    /*
    for (const auto& entry : netToInterval)
    {
        cout << "Net: " << entry.first << ", Interval: (" << entry.second.first << ", " << entry.second.second << ")" << endl;
    }
    */
    
    DirectedGraph VCG;
    for (int i = 0; i < topNodeID.size(); i++)
    {
        if (topNodeID[i] == 0 || bottomNodeID[i] == 0)
        {
            continue;
        }
        VCG.AddEdge(topNodeID[i], bottomNodeID[i]);
    }

    vector<pair<int, pair<int, int>>> sortedInterval(netToInterval.begin(), netToInterval.end());

    sort(sortedInterval.begin(), sortedInterval.end(), [](const auto& a, const auto& b) {return a.second.first < b.second.first;});
    
    /*
    for (const auto& entry : sortedInterval)
    {
        cout << "Net: " << entry.first << ", Interval: (" << entry.second.first << ", " << entry.second.second << ")" << endl;
    }
    */
    
    unordered_map<int, int> netToTrack;

    int currTrack = 1;
    int numOfNet = sortedInterval.size();
    // cout << "numOfNet = " << numOfNet << endl;
    int placedNet = 0;
    vector<bool> placed;
    placed.resize(numOfNet + 1);
    while (placedNet < numOfNet)
    {
        int watermark = -1;
        for (int i = 0; i < numOfNet; i++)
        {
            int netID = sortedInterval[i].first;
            if (sortedInterval[i].second.first > watermark && placed[netID] == false)
            {
                if (VCG.Removable(netID) == true)
                {
                    // cout << "Place " << netID << endl;
                    VCG.RemoveNode(netID);
                    placed[netID] = true;
                    placedNet++;
                    netToTrack[netID] = currTrack;
                    watermark = sortedInterval[i].second.second;
                } 
            }
        }
        currTrack++;
    }
    
    int numOfTrack = currTrack - 1;
    outputFile << "Channel density: " << numOfTrack << endl;
    for (int i = 1; i < netToInterval.size() + 1; i++)
    {
        outputFile << "Net " << i << endl;
        outputFile << "C" << numOfTrack - netToTrack[i] + 1 << " " << netToInterval[i].first << " " << netToInterval[i].second << endl;
    }
    
    return 0;
}
