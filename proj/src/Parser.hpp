#ifndef SRC_PARSER_HPP_
#define SRC_PARSER_HPP_

#include <string>
#include <vector>

#include "LabelElement.hpp"

namespace labeler
{

class Parser
{
public:
    Parser(std::string filename);

    int count;

    std::vector<LabelElement> elements;

    void write(std::string file);

    //checks, if the label aligns with its node
    inline bool aligned(LabelElement* l) {
        return !((l->x!=l->label_x && l->x!=l->label_x+l->width) || (l->y!=l->label_y && l->y!=l->label_y-l->height));
    }
};

}

#endif
