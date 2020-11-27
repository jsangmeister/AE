#include <iostream>

#include "SimpleSolver.hpp"

namespace labeler
{

int SimpleSolver::simple_eval(std::vector<LabelElement>* elements, bool print_col)
{
    int labels_set = 0;
    int collisions = 0;
    for(auto i=(*elements).begin(); i!=(*elements).end(); ++i)
    {
        if (!i->has_solution) continue;
        ++labels_set;
        for(auto j=i+1; j!=(*elements).end(); ++j)
        {
            if (j->has_solution && this->collision(&*i, &*j)) {
                --collisions;
                if(print_col) std::cout << "Collision between " << i->label << " and " 
                << j->label << std::endl;
            }        
        }
    }
    return collisions !=0 ? collisions :labels_set;
}

int SimpleSolver::simple_solution(std::vector<LabelElement>* elements)
{
    for(auto i=(*elements).begin(); i!=(*elements).end(); ++i)
    {
        i->has_solution=true;
        i->label_x1 = i->x;
        i->label_y1 = i->y + i->height;
    }
    auto labels_set = (*elements).size();

    for(auto i=(*elements).begin(); i!=(*elements).end(); ++i)
    {
        if (!i->has_solution) continue;
        for(auto j=i+1; j!=(*elements).end(); ++j)
        {
            if (j->has_solution && this->collision(&*i, &*j)) {
                j->has_solution = false;
                --labels_set;
            }
        }
    }
    return labels_set;
}

}
