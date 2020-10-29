#ifndef SRC_SOLVER_HPP_
#define SRC_SOLVER_HPP_
#include "Parser.hpp"
namespace labeler
{

// Checks, if two labels collide
inline bool collision(LabelElement* l1, LabelElement* l2);
int simple_eval(std::vector<LabelElement> elements);
int simple_solution(std::vector<LabelElement> elements);

}
#endif