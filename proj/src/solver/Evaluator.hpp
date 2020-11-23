#ifndef SRC_EVALUATOR_HPP_
#define SRC_EVALUATOR_HPP_

#include <vector>

#include "Base.hpp"
#include "../LabelElement.hpp"

namespace labeler
{

class Evaluator : virtual public Base
{
public:
    // evals in O(n^2). If overlapping labels are found and print_col set, prints them on console.
    // If it return >= 0 the solution is valid and number of labels is the returned value,
    // else the number of collisions between labels is returned*-1
    virtual int simple_eval(std::vector<LabelElement>* elements, bool print_col) = 0;
};

}

#endif
