#ifndef SRC_GUROBI_SOLVER_HPP_
#define SRC_GUROBI_SOLVER_HPP_

#include "../Solver.hpp"
#include "../Evaluator.hpp"
#include "gurobi_c++.h"

#include <vector>

namespace labeler
{

class LabelerCallback : public GRBCallback
{
    public:
        LabelerCallback(GRBVar* vars, int num_vars) : m_vars(vars), m_num_vars(num_vars) {};
    protected:
        void callback();
    private:
        GRBVar* m_vars;
        std::size_t m_num_vars;
};

class GurobiSolver : public Solver, Evaluator
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