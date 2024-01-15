#include "veb.h"

VanEmdeBoas::VanEmdeBoas(int u)
{
    universeSize = u;
    min = -1;
    max = -1;
    
    if (u <= 2)
    {
        summary = nullptr;
        clusters = vector<VanEmdeBoas*>(0, nullptr);
    }
    else
    {
        int sub_size = ceil(sqrt(u));
        summary = new VanEmdeBoas(sub_size);
        clusters = vector<VanEmdeBoas*>(sub_size, nullptr);
        for (int i = 0; i < sub_size; ++i)
        {
            clusters[i] = new VanEmdeBoas(sub_size);
        }
    }
}

int VanEmdeBoas::GetMin()
{
    return min;
}

int VanEmdeBoas::GetMax()
{
    return max;
}

void VanEmdeBoas::SetMin(int value)
{
    min = value;
}

void VanEmdeBoas::SetMax(int value)
{
    max = value;
}

int VanEmdeBoas::GenerateIndex(int x, int y)
{
    int ru = ceil(sqrt(universeSize));
    return x * ru + y;
}

void VanEmdeBoas::Insert(int key)
{
    if (min == -1)
    {
        min = key;
        max = key;
    }
    else
    {
        if (key < min)
            swap(min, key);

        if (universeSize > 2)
        {
            int sub_size = ceil(sqrt(universeSize));
            int high = key / sub_size, low = key % sub_size;
            if (clusters[high]->GetMin() == -1)
            {
                summary->Insert(high);
                clusters[high]->SetMin(low);
                clusters[high]->SetMax(low);
            }
            else
            {
                clusters[high]->Insert(low);
            }
        }

        if (key > max)
            max = key;
    }
}

void VanEmdeBoas::Delete(int key)
{
    if (max == min)
    {
        max = -1;
        min = -1;
    }
    else if (universeSize == 2)
    {
        if (key == 0)
            min = 1;
        else
            min = 0;
        max = min;
    }
    else
    {
        if (key == min)
        {
            int first_cluster = summary->GetMin();
            key = this->GenerateIndex(first_cluster, clusters[first_cluster]->GetMin());
            min = key;
        }
        
        int sub_size = ceil(sqrt(universeSize));
        int high = key / sub_size, low = key % sub_size;
        clusters[high]->Delete(low);

        if (clusters[high]->GetMin() == -1)
        {
            summary->Delete(high);

            if (key == max)
            {
                int max_in_summary = summary->GetMax();
                if (max_in_summary == -1)
                {
                    max = min;
                }
                else
                {
                    max = this->GenerateIndex(max_in_summary, clusters[max_in_summary]->GetMax());
                }
            }
        }
        else if (key == max)
        {
            max = this->GenerateIndex(high, clusters[high]->GetMax());
        }
    }
}

int VanEmdeBoas::Predecessor(int key)
{
    if (universeSize == 2)
    {
        if (key == 1 && min == 0)
            return 0;
        else
            return -1;
    }
    else if (max != -1 && key > max)
    {
        return max;
    }
    else
    {
        int sub_size = ceil(sqrt(universeSize));
        int high = key / sub_size, low = key % sub_size; 
        int min_in_cluster = clusters[high]->GetMin();
        
        int offset = 0, pred_cluster = 0;
        if (min_in_cluster != -1 && low > min_in_cluster)
        {
            offset = clusters[high]->Predecessor(low);
            return this->GenerateIndex(high, offset);
        }
        else
        {
            pred_cluster = summary->Predecessor(high);
            if (pred_cluster == -1)
            {
                if (min != -1 && key > min)
                    return min;
                else
                    return -1;
            }
            else
            {
                offset = clusters[pred_cluster]->GetMax();
                return this->GenerateIndex(pred_cluster, offset);
            }
        }
    }
}

int VanEmdeBoas::Successor(int key)
{
    if (universeSize == 2)
    {
        if (key == 0 && max == 1) 
            return 1;
        else
            return -1;
    }
    else if (min != -1 && key < min)
    {
        return min;
    }
    else
    {
        int sub_size = ceil(sqrt(universeSize));
        int high = key / sub_size, low = key % sub_size; 
        int max_in_cluster = clusters[high]->GetMax();
        
        int offset = 0, succ_cluster = 0;
        if (max_in_cluster != -1 && low < max_in_cluster)
        {
            offset = clusters[high]->Successor(low);
            return this->GenerateIndex(high, offset);
        }
        else
        {
            succ_cluster = summary->Successor(high);
            if (succ_cluster == -1)
            {
                return -1;
            }
            else
            {
                offset = clusters[succ_cluster]->GetMin();
                return this->GenerateIndex(succ_cluster, offset);
            }
        }
    }
}