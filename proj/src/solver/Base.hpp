#ifndef SRC_BASE_HPP_
#define SRC_BASE_HPP_

#include "../LabelElement.hpp"

namespace labeler
{

class Base
{
protected:
    // Checks, if two labels collide
    inline bool collision(LabelElement* l1, LabelElement* l2) {
        // both labels must be active
        if (!(l1->has_solution && l2->has_solution)) return false;

        // This statement is explained in docs/collision_explain.txt
        return ((l1->label_x1-l2->label_x2)*(l1->label_x2-l2->label_x1) < 0)
            && ((l1->label_y1-l2->label_y2)*(l1->label_y2-l2->label_y1) < 0);
    }
};

}

#endif
