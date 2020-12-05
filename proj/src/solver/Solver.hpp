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
    // Solves the instance for the given elements and returns a vector of ints.
    // first index is the amount of set labels, the other positions are free to use for the algorithms.
    // args can be used to pass additional arguments to the solver.
    virtual std::vector<long unsigned int> solve(std::vector<LabelElement>* elements, std::vector<double> args) = 0;
};

}

#endif
