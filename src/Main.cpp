#include <getopt.h>
#include <iostream>

#include "Parser.hpp"
#include "Solver.hpp"

using namespace labeler;

int main(int argc, char** argv)
{
    std::string in_arg, out_arg, eval_arg, file_to_parse;
    static struct option long_options[] = {
        {"in", required_argument, NULL, 1},
        {"out", required_argument, NULL, 2},
        {"eval", required_argument, NULL, 3},
        {0, 0, 0, 0}
    };
    while (1)
    {
        int c = getopt_long_only(argc, argv, "", long_options, nullptr);
        if (c == -1)
            break;

        switch (c) {
            case 1:
                in_arg = file_to_parse = optarg;
                break;
            case 2:
                out_arg = optarg;
                break;
            case 3:
                eval_arg = file_to_parse = optarg;
                break;
            default:
                printf("getopt returned invalid character code 0%o\n", c);
        }
    }

    if (
        (in_arg.empty() && out_arg.empty() && eval_arg.empty()) ||
        (!in_arg.empty() && (out_arg.empty() || !eval_arg.empty())) ||
        (!out_arg.empty() && (in_arg.empty() || !eval_arg.empty())) ||
        (!eval_arg.empty() && (!in_arg.empty() || !out_arg.empty()))
    ) {
        std::cout << "Usage: labeler [-in input_file -out output_file] [-eval file]" << std::endl <<
            "Options:" << std::endl <<
            "-in input_file     Read data from input_file" << std::endl <<
            "-out output_file   After executing the algorithm, write the solution to output_file" << std::endl <<
            "-eval file         Verify the problem and solution given in file" << std::endl;
        std::exit(0);
    }

    Parser parser = Parser(file_to_parse);
    std::cout << "Parsing successful." << std::endl;

    if(!eval_arg.empty())
    {   
        // This of course could be moved into simple_eval or into the Parser constructor
        for(std::size_t i=0; i<parser.elements.size(); ++i) {
            auto l = &(parser.elements[i]);
            if (l->has_solution && !aligned(l))
            {
                throw std::runtime_error("ERROR: Invalid entry on line " + std::to_string(i+1)
                + ", label not correctly aligned");
            }
        }

        int val = simple_eval(&parser.elements, true);
        
        if (val < -1)
        {
            std::cout << "Solution is not valid, " << -val << "overlappings found.";
        }
        else if (val == -1)
        {
            std::cout << "Solution is not valid, 1 overlapping found.";
        }
        else if (val == 0)
        {
            std::cout << "Solution is valid, but only because no labels are set." << std::endl;
        }
        else if (val == 1)
        {
            std::cout << "Solution is valid with 1 label set." << std::endl;
        }
        else
        {
            std::cout << "Solution is valid with " << val << " labels set." << std::endl;
        }
        return 0;
    }

    int labels_set = simple_solution(&parser.elements);

    std::cout << "Solution with " << labels_set << " labels found." << std::endl;

    parser.write(out_arg);

}
