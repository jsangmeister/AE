/**
 * Uses Simulated Annealing as heuristic
 **/

#include "../Solver.hpp"
#include "../Evaluator.hpp"

#include <vector>

namespace labeler {


class SimulAnSolver : public Solver, Evaluator
{
    public:

        /**
         *  Standard Solver, also sets init_active to the number of initially active label 
         *  and max_active to the highest number reached during runtime
         * @param args  if vector of size 3, args[0] is starting temperature, args[2] max_tries in one 
         *              temperature and if args[2]==0 labels are all initialized off instead of random
         */
        std::vector<long unsigned int> solve(std::vector<LabelElement>* elements, std::vector<double> args);
        int eval(std::vector<LabelElement>* elements, bool print_col);
    private:
        void intToPos(LabelElement& el, int pos);
        void permVector(std::vector<int>* vec);
        void addToStack(std::vector<std::pair<int, int>>* stack, LabelElement* el, int i);
};

}