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
    virtual int eval(std::vector<LabelElement>* elements, bool print_col) = 0;
};

}

#endif
