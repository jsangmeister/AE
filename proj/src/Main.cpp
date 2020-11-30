#include <getopt.h>
#include <iostream>
#include <chrono>
#include <sstream>

#include "Parser.hpp"
#include "solver/solver/SimpleSolver.hpp"
#include "solver/solver/SimulAn.hpp"

using namespace labeler;

void solve(Parser parser);
void solveSimulAn(Parser& parser, std::vector<double> confVec);
void eval(Parser parser);
void parseConfString(std::vector<double>& vec, std::string confString);


int main(int argc, char** argv)
{
    std::string in_arg, out_arg, eval_arg, file_to_parse, sol_arg, conf_arg;
    static struct option long_options[] = {
        {"in", required_argument, NULL, 1},
        {"out", required_argument, NULL, 2},
        {"eval", required_argument, NULL, 3},
        {"sol", required_argument, NULL, 4}, 
        {"conf", required_argument, NULL, 5},
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
            case 4:
                sol_arg = optarg;
                break;
            case 5:
                conf_arg = optarg;
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
        std::cout << "Usage: labeler [-in input_file -out output_file] [-eval file] [-sol solver] [-conf conf_string]" << std::endl <<
            "Options:" << std::endl <<
            "-in input_file     Read data from input_file" << std::endl <<
            "-out output_file   After executing the algorithm, write the solution to output_file" << std::endl <<
            "-eval file         Verify the problem and solution given in file" << std::endl <<
            "-sol solver        Choose a solver, uses very basic solver if not set, available are:" << std::endl <<
            "                       sa  Simulated Annealing" << std::endl <<
            "-conf conf_string  A string to parse a solver configuration" << std::endl;
        std::exit(0);
    }

    Parser parser = Parser(file_to_parse);
    //std::cout << "Parsing successful." << std::endl;

    if(!eval_arg.empty()) {
        eval(parser);
    } else {
        if(sol_arg.empty())
            solve(parser);
        else if (sol_arg.compare("sa") == 0)
        {
            std::vector<double> confVec;
            if (conf_arg.empty())
            {
                solveSimulAn(parser, confVec);
            }
            else
            {
                parseConfString(confVec, conf_arg);
                if (confVec.size() != 3)
                {
                    std::cout << "conf_string not valid for sa: " << conf_arg << std::endl;
                    return 0;
                }
                solveSimulAn(parser, confVec);
            }
        }
        else
        {
            std::cout << sol_arg << " is not a solver" << std::endl;
            return 0;
        }
        parser.write(out_arg);
    }
    return 0;   
}

void solve(Parser parser) {
    SimpleSolver solver = SimpleSolver();

    auto start = std::chrono::high_resolution_clock::now();
    int labels_set = solver.simple_solution(&parser.elements);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Solution with " << labels_set << " labels found. (" << duration << "ms)" << std::endl;
}

void solveSimulAn(Parser& parser, std::vector<double> args)
{
    int init_acvtive, max_active;
    auto solver = SimulAnSolver();
    auto start = std::chrono::high_resolution_clock::now();
    int labels_set = solver.solve(&parser.elements, args, init_acvtive, max_active);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    //std::cout << "Solution with " << labels_set << " labels found. (" << duration << "ms)" << std::endl;
    std::cout << labels_set << ", " << duration << ", " << init_acvtive << ", " << max_active 
    << "" << std::endl; 
}

void eval(Parser parser) {
    SimpleSolver ev = SimpleSolver();

    // This of course could be moved into simple_eval or into the Parser constructor
    for(std::size_t i=0; i<parser.elements.size(); ++i) {
        auto l = &(parser.elements[i]);
        if (l->has_solution && !parser.aligned(l))
        {
            throw std::runtime_error("ERROR: Invalid entry on line " + std::to_string(i+1)
            + ", label not correctly aligned");
        }
    }

    int val = ev.simple_eval(&parser.elements, true);

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
}

void parseConfString(std::vector<double>& vec, std::string confString)
{
    std::stringstream strm(confString);
    double val;
    while(strm >> val)
    {
        vec.push_back(val);
    }
}