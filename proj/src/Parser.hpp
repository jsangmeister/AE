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
    bool has_solution;
    int label_x1;
    int label_y1;
    /* x2 and y2 are the coordinates of the bottom right corner.
    *   Yes, we can calculate them from x,y, width and length, but do
    *   we really want to every time?
    */
    int label_x2;
    int label_y2;

    std::string toString();
};

class Parser
{
public:
    Parser(std::string filename);

    int count;

    std::vector<LabelElement> elements;

    void write(std::string file);
};

//checks, if the label aligns with its node
inline bool aligned(LabelElement* l)
{
    return !((l->x!=l->label_x1 && l->x!=l->label_x2) || (l->y!=l->label_y1 && l->y!=l->label_y2));
}

}
#endif