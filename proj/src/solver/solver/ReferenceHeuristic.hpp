#ifndef SRC_REFERENCEHEURISTIC_SOLVER_HPP_
#define SRC_REFERENCEHEURISTIC_SOLVER_HPP_

#include"../Solver.hpp"
#include<vector>

namespace labeler {


class ReferenceHeuristic: public Solver
{
    public:
        std::vector<long unsigned int> solve(std::vector<LabelElement>* elements, std::vector<double> args);
    private:
        void intToPos(LabelElement& el, int pos);
        void permVector(std::vector<long unsigned int>* vec);
};

}

#endif