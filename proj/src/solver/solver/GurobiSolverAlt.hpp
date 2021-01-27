#ifndef SRC_GUROBI_SOLVER_ALT_HPP_
#define SRC_GUROBI_SOLVER_ALT_HPP_

#include "../Solver.hpp"
#include "../Evaluator.hpp"
#include "GurobiSolver.hpp"
#include "gurobi_c++.h"

#include <vector>

namespace labeler
{

#ifndef CONFLICTGRAPHTYPE
#define CONFLICTGRAPHTYPE
typedef std::vector<std::vector<std::vector<size_t>>> ConflictGraph;
#endif

class GurobiSolverAlt: public Solver, Evaluator
{
    public:

        /**
         * @param args 
         */
        std::vector<long unsigned int> solve(std::vector<LabelElement>* elements, std::vector<double> args);
        int eval(std::vector<LabelElement>* elements, bool print_col);
    private:
        void intToPos(LabelElement& el, int pos);
        std::vector<std::vector<std::vector<std::size_t>>> makeConflictGraph(std::vector<LabelElement>& elements);
};

}
#endif