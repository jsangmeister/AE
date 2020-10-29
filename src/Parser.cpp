#include "Parser.hpp"

#include <sstream>
#include <string>
#include <fstream>

namespace labeler
{

Parser::Parser(std::string filename) : elements() {
    std::ifstream infile(filename);
    std::string line;
    int line_number = 0;
    while (std::getline(infile, line))
    {
        line_number++;
        std::istringstream iss(line);

        if (line_number == 1) {
            if (!(iss >> count)) {
                throw std::runtime_error("ERROR: The first line must give the total number of entries.");
            }
        } else {
            if (line_number - 1 > count) {
                throw std::runtime_error("ERROR: More entries than stated in first line.");
            }
            int x, y, width, height, has_solution, label_x, label_y;
            std::string label;
            if (!(iss >> x >> y >> width >> height >> label >> has_solution >> label_x >> label_y)) {
                throw std::runtime_error("ERROR: Invalid entry on line " + std::to_string(line_number));
            }

            LabelElement element = {x, y, width, height, label, has_solution, label_x, label_y, 
            label_x + width, label_y - height};
            elements.push_back(element);
        }
    }
    if (line_number - 1 < count) {
        throw std::runtime_error("ERROR: Less entries than stated in first line.");
    }
}

}
