#ifndef SRC_PARSER_HPP_
#define SRC_PARSER_HPP_

#include <string>
#include <vector>

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
    int has_solution;
    int label_x;
    int label_y;
};

class Parser
{
public:
    Parser(std::string filename);

    int count;

    std::vector<LabelElement> elements;
};

}

#endif