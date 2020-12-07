#include "ReferenceHeuristic.hpp"
#include<iostream>
#include <stdlib.h>
#include <time.h> 
#include <math.h>

namespace labeler{


void ReferenceHeuristic::intToPos(LabelElement& el, int pos)
{
    if(pos == -1)
    {
        el.has_solution = false;
        return;
    }
    el.has_solution = true;
    el.label_x = el.x - (pos/2) * el.width;
    el.label_y = el.y + (pos%2) * el.height;
}


void ReferenceHeuristic::permVector(std::vector<long unsigned >* vec)
{
    for(int i=vec->size()-1; i>=1; i--)
    {
        int j = rand() % (i+1);
        int tmp = (*vec)[i];
        (*vec)[i] = (*vec)[j];
        (*vec)[j] = tmp;
    }
}

std::vector<long unsigned int> ReferenceHeuristic::solve(std::vector<LabelElement>* elements, std::vector<double> args)
{
    srand(time(NULL));
    for(auto e: *elements)
    {
        e.has_solution = false;
    }
    unsigned long int active = 0;
    for(auto i=(*elements).begin(); i!=(*elements).end(); ++i)
    {
        std::vector<long unsigned  int> positions{0, 1, 2, 3};
        permVector(&positions);
        bool found;
        for (auto pos: positions)
        {
            found = true;
            intToPos(*i, pos);
            for (auto j=(*elements).begin(); j!=i; ++j)
            {
                if(collision(*i, *j))
                {
                    found = false;
                    break;
                }
            }
            if (found)
            {
                break;
            }
        }
        if (!found)
        {
            i->has_solution = false;
        }
        else
        {
            active++;
        }
    }
    return std::vector<long unsigned int>{active};
}

}