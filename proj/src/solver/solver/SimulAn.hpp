/**
 * Uses Simulated Annealing as heuristic
 **/

#include"../Base.hpp"
#include<vector>

namespace labeler {


struct ElementWrapper
{
    LAB_POS lp;
    LabelElement* labEl;
};

struct LabelConflict
{
    std::vector<int> conflicts[4]; 
};


class SimulAnSolver: public Base
{
    public:
        int solve(std::vector<LabelElement>* elements);
        int simple_eval(std::vector<LabelElement>* elements, bool print_col);
};

}