#ifndef SRC_SOLVER_HPP_
#define SRC_SOLVER_HPP_
#include "Parser.hpp"
namespace labeler
{

// Checks, if two labels collide
inline bool collision(LabelElement* l1, LabelElement* l2)
{
    // both labels must be active
    if (!(l1->has_solution && l2->has_solution)) return false;

    // This statement is explained in docs/collision_explain.txt
    return ((l1->label_x1-l2->label_x2)*(l1->label_x2-l2->label_x1) < 0)
        && ((l1->label_y1-l2->label_y2)*(l1->label_y2-l2->label_y1) < 0);
}


// evals in O(n^2). If overlapping labels are found and print_col set, prints them on console.
// If it return >= 0 the solution is valid and number of labels is the returned value,
// else the number of collisions between labels is returned*-1
int simple_eval(std::vector<LabelElement>* elements, bool print_col);

// Generates a very simple Solution by just setting every label to the right top
// of it's point, then deactivates every colliding label. The lower element in the 
// vector always prevails. Might generate very bad (but always valid) solutions.
int simple_solution(std::vector<LabelElement>* elements);

}
#endif