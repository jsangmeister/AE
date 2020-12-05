#include <iostream>

#include "SimpleSolver.hpp"

namespace labeler
{

int SimpleSolver::eval(std::vector<LabelElement>* elements, bool print_col)
{
    int labels_set = 0;
    int collisions = 0;
    for(auto i=elements->begin(); i!=elements->end(); ++i)
    {
        if (!i->has_solution) continue;
        ++labels_set;
        for(auto j=i+1; j!=elements->end(); ++j)
        {
            if (j->has_solution && this->collision(*i, *j)) {
                --collisions;
                if(print_col) std::cout << "Collision between " << i->label << " and " 
                << j->label << std::endl;
            }        
        }
    }
    return collisions !=0 ? collisions :labels_set;
}

std::vector<long unsigned int> SimpleSolver::solve(std::vector<LabelElement>* elements, std::vector<double> args)
{
    for(auto i=elements->begin(); i!=elements->end(); ++i)
    {
        i->has_solution=true;
        i->label_x = i->x;
        i->label_y = i->y + i->height;
    }
    auto labels_set = elements->size();

    for(auto i=elements->begin(); i!=elements->end(); ++i)
    {
        if (!i->has_solution) continue;
        for(auto j=i+1; j!=elements->end(); ++j)
        {
            if (j->has_solution && this->collision(*i, *j)) {
                j->has_solution = false;
                --labels_set;
            }
        }
    }
    return std::vector<long unsigned int>{labels_set};
}

}
