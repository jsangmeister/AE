#include "Parser.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

namespace labeler
{

Parser::Parser(std::string filename) : elements() {
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        throw std::runtime_error("ERROR: File " + filename + " could not be opened");
    }

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
            int x, y, width, height, label_x, label_y;
            bool has_solution;
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

void Parser::write(std::string file)
{
    std::ofstream os;
    os.open(file);
    os << elements.size();
    for(auto e : elements)
    {
        os << "\n" << e.toString();
    }
    os.close();
}

//Maybe just overload <<?
std::string LabelElement::toString(){
    auto w = " ";
    std::stringstream out;
    out << x << w << y << w << width << w << height << w << label << w
    << has_solution << w << label_x1 << w << label_y1;
    return out.str();
}

}
