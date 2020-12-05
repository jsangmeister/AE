#ifndef SRC_LABEL_ELEMENT_HPP_
#define SRC_LABEL_ELEMENT_HPP_

#include <string>
#include <sstream>

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
    // top left corner
    int label_x;
    int label_y;

    inline std::string toString() {
        auto w = " ";
        std::stringstream out;
        out << x << w << y << w << width << w << height << w << label << w
        << has_solution << w << label_x << w << label_y;
        return out.str();
    };
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
