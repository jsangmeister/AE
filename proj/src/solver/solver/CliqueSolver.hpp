#ifndef SRC_CLIQUE_SOLVER_HPP_
#define SRC_CLIQUE_SOLVER_HPP_

#include "../Solver.hpp"
#include "../Evaluator.hpp"
#include "gurobi_c++.h"

#include <vector>

namespace labeler
{

#ifndef CONFLICTGRAPHTYPE
#define CONFLICTGRAPHTYPE
typedef std::vector<std::vector<std::vector<size_t>>> ConflictGraph;
#endif

class CliqueSolver : public Solver, Evaluator
{
    public:

        /**
         * @param args 
         */
        std::vector<long unsigned int> solve(std::vector<LabelElement>* elements, std::vector<double> args);
        int eval(std::vector<LabelElement>* elements, bool print_col);
    private:
        void intToPos(LabelElement& el, int pos);
};

}
#endif