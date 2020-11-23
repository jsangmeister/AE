#ifndef SRC_SIMPLE_SOLVER_HPP_
#define SRC_SIMPLE_SOLVER_HPP_

#include "../Solver.hpp"
#include "../Evaluator.hpp"

namespace labeler
{

class SimpleSolver : public Solver, Evaluator
{
public:
    int simple_eval(std::vector<LabelElement>* elements, bool print_col);
    int simple_solution(std::vector<LabelElement>* elements);
};

}

#endif
