#include <getopt.h>
#include <iostream>

#include "Parser.hpp"

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
}
