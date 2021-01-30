#include "GurobiSolverPoint.hpp"
#include <algorithm>

namespace labeler
{

void GurobiSolverPoint::intToPos(LabelElement& el, int pos)
{
    if(pos == -1)
    {
        el.has_solution = false;
        return;
    }
    el.has_solution = true;
    el.label_x = el.x - (pos/2) * el.width;
    el.label_y = el.y + (pos%2) * el.height;
}


int GurobiSolverPoint::eval(std::vector<LabelElement>* elements, bool print_col)
{
    int labels_set = 0;
    int collisions = 0;
    for(auto i=elements->begin(); i!=elements->end(); ++i)
    {
        if (!i->has_solution) continue;
        ++labels_set;
        for(auto j=i+1; j!=elements->end(); ++j)
        {
            if (j->has_solution && this->collision(*i, *j)) {
                --collisions;
                if(print_col) std::cout << "Collision between " << i->label << " and " 
                << j->label << std::endl;
            }        
        }
    }
    return collisions !=0 ? collisions :labels_set;
}

std::vector<long unsigned int> GurobiSolverPoint::solve(std::vector<LabelElement>* elements, std::vector<double> args)
{
    // args[...] meaning:
    // 0:  whether or not to use the heuristic callback (default: 0)
    // 1:  threshold to not set the label in the heuristic callback (default: 0.5)
    // 2:  ConcurrentMIP (default: 1)
    // 3:  MIPFocus (default: 0)
    // 4:  Method (default: -1)
    // 5:  Cuts (default: -1)
    // 6:  Presolve (default: -1)

    bool use_conflict_graph_heuristic = false;
    ConflictGraph conflicts;
    if (args.size() > 0 && args[0] > 0) 
    {
        // std::cout << "Using heuristic callback" << std::endl;
        use_conflict_graph_heuristic = true;
        conflicts = ConflictGraph(elements->size(), std::vector<std::vector<size_t>>(4));
    }

    GRBEnv env = GRBEnv(true);
    //env.set("LogFile", "labeler_gurobi.log");
    env.set(GRB_IntParam_OutputFlag, 0);

    // parameter tuning
    if (args.size() > 2) {
        // std::cout << "ConcurrentMIP=" << args[2] << std::endl;
        env.set("ConcurrentMIP", std::to_string(args[2]));
    }
    if (args.size() > 3) {
        // std::cout << "MIPFocus=" << args[3] << std::endl;
        env.set("MIPFocus", std::to_string(args[3]));
    }
    if (args.size() > 4) {
        // std::cout << "Method=" << args[4] << std::endl;
        env.set("Method", std::to_string(args[4]));
    }
    if (args.size() > 5) {
        // std::cout << "Cuts=" << args[5] << std::endl;
        env.set("Cuts", std::to_string(args[5]));
    }
    if (args.size() > 6) {
        // std::cout << "Presolve=" << args[6] << std::endl;
        env.set("Presolve", std::to_string(args[6]));
    }

    env.start();
    GRBModel model = GRBModel(env);
    model.set(GRB_DoubleParam_TimeLimit, 600.0);

    //Build Variables
    std::size_t num_vars = elements->size() * 4;
    GRBVar* variables = model.addVars(num_vars, GRB_BINARY);

    //Build objective function
    GRBLinExpr obj = 0;
    double coefficient = 1.0;
    for (std::size_t i=0; i<num_vars; i++)
    {
        obj.addTerms(&coefficient, variables+i, 1);
    }
    model.setObjective(obj, GRB_MAXIMIZE);


    //Build Constraints
    std::vector<GRBLinExpr> pointsums(elements->size());
    for (std::size_t i = 0; i<elements->size(); i++)
    {
        pointsums[i] += variables[i*4] + variables[i*4+1] + variables[i*4+2] + variables[i*4+3];
    }
    std::vector<GRBConstr> conflictConstr;

    std::size_t range4[] = {0, 1, 2, 3}; 
    for (std::size_t i = 0; i<elements->size(); i++)
    {
        LabelElement e = elements->at(i);
        for (std::size_t j = i+1; j<elements->size(); j++)
        {
            bool first_loop = true;
            for (int pos_e: range4)
            {
                intToPos(e, pos_e);
                LabelElement f = elements->at(j);
                //std::cout << e.label << " gegen " << f.label << std::endl;
                if (e.label_x < f.x && e.label_x + e.width > f.x &&
                    e.label_y > f.y && e.label_y - e.height < f.y)
                {
                    //std::cout << e.label << " " << e.label_x << "," << e.label_y << " frisst " << f.label << std::endl;
                    pointsums[j] += variables[i*4+pos_e];
                    continue;
                }
                for (int pos_f: range4)
                {
                    intToPos(f, pos_f);
                    if (first_loop &&
                        f.label_x < e.x && f.label_x + f.width > e.x &&
                        f.label_y > e.y && f.label_y - f.height < e.y)
                    {
                        //std::cout << f.label << " " << f.label_x << "," << f.label_y << " frisst " << e.label << std::endl;
                        pointsums[i] += variables[j*4+pos_f];
                        continue;
                    }
                    if(collision(e,f))
                    {
                        auto loopConflictConstr = model.addConstr(variables[i*4+pos_e] + variables[j*4+pos_f] <= 1);
                        conflictConstr.push_back(loopConflictConstr);
                        if (use_conflict_graph_heuristic)
                        {
                            //This doesn not(!) build a complete conflict graph. Each node does only
                            // know with which precursor element it collides! 
                            conflicts[j][pos_f].push_back(i*4+pos_e);
                            
                            // The snippet below would complete the conflict graph building
                            //conflicts[i][pos_e].push_back(j*4+pos_f);
                        }
                    }
                }
                first_loop = false;
            }
        }
        elements->at(i).has_solution=false;
    }


    std::vector<GRBConstr> pointConstr;
    for (auto linex : pointsums)
    {
        pointConstr.push_back(model.addConstr(linex <= 1));
    }
    

    std::size_t active_label = 0;

    // set callback
    if (use_conflict_graph_heuristic)
    {
        double ignore_below = 0.5;
        if (args.size() > 1)
        {
            ignore_below = args[1];
        }
        LabelerCallback cb(variables, num_vars, conflicts, ignore_below);
        model.setCallback(&cb);
        model.optimize();
    }
    else
    {
        model.optimize();
    }
    for (std::size_t i=0; i<elements->size(); i++)
    {
        for (auto j: range4)
        {
            if(variables[i*4+j].get(GRB_DoubleAttr_X)>0.9)
            {
                active_label++;
                intToPos(elements->at(i), j);
                break;
            }
        }
    }

    // std::cout << model.get(GRB_IntAttr_Status) << "\t";

    return std::vector<long unsigned int>{active_label};
}

}
