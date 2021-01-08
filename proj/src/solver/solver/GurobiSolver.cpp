#include "GurobiSolver.hpp"
#include <algorithm>

namespace labeler
{

void LabelerCallback::callback()
{
    // call heuristic on every newly created node
    if (where == GRB_CB_MIPNODE) {
        //std::cout << "**** New node ****" << std::endl;
        //std::cout << (getIntInfo(GRB_CB_MIPNODE_STATUS) == GRB_OPTIMAL) << std::endl;
        // I guess it's not necessary to execute the heuristic on non-optimal nodes
        // - at least it is like that in the examples...
        //
        // Not exactly, the reason is that getNodeRel only has a valid relaxation if GRB_OPTIMAL
        if (getIntInfo(GRB_CB_MIPNODE_STATUS) == GRB_OPTIMAL) {
            double* x = getNodeRel(m_vars, m_num_vars);
            // We iterate over x in steps of size 4, therefore, we always look at the 4 variables
            // corresponding to one point in the labeling problem
            for (size_t i=0; i<m_num_vars; i+=4)
            {
                // We sort the indices of the 4 variables according to their relaxated solution in descending order
                std::vector<size_t> index_sorted{0, 1, 2, 3};
                std::sort(index_sorted.begin(), index_sorted.end(),
                    [&](size_t i1, size_t i2){ return x[i+i1] > x[i+i2];});
                size_t best_index = -1;
                //Now we find the variable with the highest solution that is not blocked by a preceeding variable
                for (auto s_i : index_sorted)
                {
                    bool matches = true;
                    // ignore values below threshold
                    if (x[i+s_i]<=ignore_below)
                    {
                        continue;
                    }
                    for (auto conflict_index : m_conflicts[i/4][s_i])
                    {
                        if (x[conflict_index] == 1.0)
                        {
                            matches = false;
                            break;
                        }
                    }
                    if(matches)
                    {
                        best_index = s_i;
                        break;
                    }
                }
                // Set the variable at the best index to 1, the others to 0
                // If best variable is still -1, all variables are set to 0
                for (size_t j=0; j<4; j++)
                {
                    if (j==best_index)
                    {
                        x[i+j]=1.0;
                    }
                    else
                    {
                        x[i+j]=0.0;
                    }
                }
            }
            setSolution(m_vars, x, m_num_vars);
            delete[] x;
        }
    }
}

void GurobiSolver::intToPos(LabelElement& el, int pos)
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


int GurobiSolver::eval(std::vector<LabelElement>* elements, bool print_col)
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

std::vector<long unsigned int> GurobiSolver::solve(std::vector<LabelElement>* elements, std::vector<double> args)
{

    bool use_conflict_graph_heuristic = false;
    ConflictGraph conflicts;
    if (args.size() > 0 && args[0] > 0) 
    {
        use_conflict_graph_heuristic = true;
        conflicts = ConflictGraph(elements->size(), std::vector<std::vector<size_t>>(4));
    }

    GRBEnv env = GRBEnv(true);
    //env.set("LogFile", "labeler_gurobi.log");
    env.set(GRB_IntParam_OutputFlag, 0);
    env.start();
    GRBModel model = GRBModel(env);

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
    std::vector<GRBConstr> labelConstr;
    std::vector<GRBConstr> conflictConstr;

    std::size_t range4[] = {0, 1, 2, 3}; 
    for (std::size_t i = 0; i<elements->size(); i++)
    {
        GRBConstr loopLabelConstr = model.addConstr(variables[i*4] + variables[i*4+1] + variables[i*4+2] + variables[i*4+3] <= 1);
        labelConstr.push_back(loopLabelConstr);

        LabelElement e = elements->at(i);
        for (int pos_e: range4)
        {
            intToPos(e, pos_e);
            for (std::size_t j = i+1; j<elements->size(); j++)
            {
                LabelElement f = elements->at(j);
                for (int pos_f: range4)
                {
                    intToPos(f, pos_f);
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
            }
        }
        elements->at(i).has_solution=false;
    }

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
    

    std::size_t active_label = 0;
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

    return std::vector<long unsigned int>{active_label};
}

}
