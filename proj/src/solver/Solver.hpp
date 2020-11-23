#ifndef SRC_SOLVER_HPP_
#define SRC_SOLVER_HPP_

#include <iostream>
#include <vector>

#include "Base.hpp"
#include "../LabelElement.hpp"

namespace labeler
{

class Solver : virtual public Base
{
public:
    // Generates a very simple Solution by just setting every label to the right top
    // of it's point, then deactivates every colliding label. The lower element in the 
    // vector always prevails. Might generate very bad (but always valid) solutions.
    virtual int simple_solution(std::vector<LabelElement>* elements) = 0;
};

}

#endif
