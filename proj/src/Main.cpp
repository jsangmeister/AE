#include <getopt.h>
#include <iostream>
#include <chrono>
#include <sstream>

#include "Parser.hpp"
#include "solver/solver/SimpleSolver.hpp"
#include "solver/solver/SimulAn.hpp"
#include "solver/solver/ReferenceHeuristic.hpp"
#include "solver/solver/RulesSolver.hpp"
#include "solver/Solver.hpp"

using namespace labeler;

void solve(Parser* parser, Solver* solver, std::vector<double> args);
void eval(Parser* parser);
std::vector<double> parseConfString(std::string confString);


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
    std::cout << conf_arg << std::endl;

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

    Parser* parser = new Parser(file_to_parse);

    if(!eval_arg.empty()) {
        eval(parser);
    } else {
        auto args = parseConfString(conf_arg);
        Solver* solver;
        if (sol_arg == "sa") {
            solver = new SimulAnSolver();
        } else if (sol_arg == "rules") {
            solver = new RulesSolver();
        } else if (sol_arg == "simple") {
            solver = new SimpleSolver();
        } else if (sol_arg == "ref") {
            solver = new ReferenceHeuristic();
        } else {
            throw std::runtime_error("Invalid solver: " + sol_arg);
        }
        solve(parser, solver, args);
        parser->write(out_arg);
    }
    return 0;   
}

void solve(Parser* parser, Solver* solver, std::vector<double> args) {
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<long unsigned int> result = solver->solve(&parser->elements, args);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Solution with " << result[0] << " labels found (" << duration << "ms)." << std::endl;
    if (result.size() > 1) {
        std::cout << "Output:";
        for (int i = 1; i < result.size(); i++) {
            std::cout << result[i];
        }
        std::cout << std::endl;
    }
}

void eval(Parser* parser) {
    SimpleSolver ev = SimpleSolver();

    // This of course could be moved into simple_eval or into the Parser constructor
    for(std::size_t i=0; i<parser->elements.size(); ++i) {
        auto l = &(parser->elements[i]);
        if (l->has_solution && !parser->aligned(l))
        {
            throw std::runtime_error("ERROR: Invalid entry on line " + std::to_string(i+1)
            + ", label not correctly aligned");
        }
    }

    int val = ev.eval(&parser->elements, true);

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

std::vector<double> parseConfString(std::string confString)
{
    std::vector<double> vec;
    std::stringstream strm(confString);
    double val;
    while(strm >> val)
    {
        vec.push_back(val);
    }
    return vec;
}
