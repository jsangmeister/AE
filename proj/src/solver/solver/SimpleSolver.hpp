#ifndef SRC_SIMPLE_SOLVER_HPP_
#define SRC_SIMPLE_SOLVER_HPP_

#include "../Solver.hpp"
#include "../Evaluator.hpp"

namespace labeler
{

class SimpleSolver : public Solver, Evaluator
{
public:
    // evals in O(n^2). If overlapping labels are found and print_col set, prints them on console.
    // If it return >= 0 the solution is valid and number of labels is the returned value,
    // else the number of collisions between labels is returned*-1
    int eval(std::vector<LabelElement>* elements, bool print_col);
    // Generates a very simple Solution by just setting every label to the right top
    // of it's point, then deactivates every colliding label. The lower element in the 
    // vector always prevails. Might generate very bad (but always valid) solutions.
    // args are ignored.
    std::vector<long unsigned int> solve(std::vector<LabelElement>* elements, std::vector<double> args);
};

}

#endif
