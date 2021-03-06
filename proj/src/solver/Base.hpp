#ifndef SRC_BASE_HPP_
#define SRC_BASE_HPP_

#include "../LabelElement.hpp"

namespace labeler
{

class Base
{
protected:
    // Checks, if two labels collide
    inline bool collision(LabelElement l1, LabelElement l2) {
        // both labels must be active
        if (!(l1.has_solution && l2.has_solution)) return false;

        // This statement is explained in docs/collision_explain.txt
        return ((l1.label_x-l2.label_x-l2.width)*(l1.label_x+l1.width-l2.label_x) < 0)
            && ((l1.label_y-l2.label_y+l2.height)*(l1.label_y-l1.height-l2.label_y) < 0);
    }
};

}

#endif
