#ifndef SRC_LABEL_ELEMENT_HPP_
#define SRC_LABEL_ELEMENT_HPP_

#include<string>

namespace labeler
{

// see docs for meaning of fields
struct LabelElement
{
    int x;
    int y;
    int width;
    int height;
    std::string label;
    bool has_solution;
    int label_x1;
    int label_y1;
    /* x2 and y2 are the coordinates of the bottom right corner.
    *   Yes, we can calculate them from x,y, width and length, but do
    *   we really want to every time? Yes, we want
    */
    //int label_x2;
    //int label_y2;

    std::string toString();
};

enum LAB_POS
{
    LAB_NOTSET = -1,
    LAB_BOTR = 0,
    LAB_TOPR = 1,
    LAB_BOTL = 2,
    LAB_TOPL = 3
};

}

#endif
