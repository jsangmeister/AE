#ifndef SRC_GUROBI_SOLVER_EXT_HPP_
#define SRC_GUROBI_SOLVER_EXT_HPP_

#include "../Solver.hpp"
#include "../Evaluator.hpp"
#include "gurobi_c++.h"
#include "LabelCandidate.hpp"

#include <vector>
#include <numeric>

namespace labeler
{

class CutCallback : public GRBCallback
{
    public:
        CutCallback(GRBVar* vars, int num_vars, std::vector<std::vector<size_t>> conflicts)
        : m_vars(vars), m_num_vars(num_vars),  m_conflicts(conflicts), m_indices(conflicts.size()) {
            std::iota(std::begin(m_indices), std::end(m_indices), 0);
            std::sort(this->m_indices.begin(), this->m_indices.end(), [&](long unsigned int k, long unsigned int l) {
                return this->m_conflicts[k].size() > this->m_conflicts[l].size();
            });
        };
    protected:
        void callback();
    private:
        GRBVar* m_vars;
        std::size_t m_num_vars;
        std::vector<std::vector<size_t>> m_conflicts;
        std::vector<long unsigned int> m_indices;
        std::vector<std::pair<GRBTempConstr, std::vector<size_t>>> m_constr;
};

class GurobiSolverExt : public Solver
{
    public:

        /**
         * @param args 
         */
        std::vector<long unsigned int> solve(std::vector<LabelElement>* elements, std::vector<double> args);
    private:
        void intToPos(LabelElement& el, int pos);
        std::tuple<int, int> getCandidatePosition(LabelCandidate* c);
        std::vector<std::vector<std::vector<std::size_t>>> makeConflictGraph(std::vector<LabelElement>& elements);
};

}
#endif