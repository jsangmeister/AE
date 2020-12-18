#include "GurobiSolver.hpp"

namespace labeler
{

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

    GRBEnv env = GRBEnv(true);
    //env.set("LogFile", "labeler_gurobi.log");
    env.set(GRB_IntParam_OutputFlag, 0);
    env.start();
    GRBModel model = GRBModel(env);

    //Build Variables
    GRBVar* variables = model.addVars(elements->size() * 4, GRB_BINARY);

    //Build objective function
    GRBLinExpr obj = 0;
    double coefficient = 1.0;
    for (std::size_t i=0; i<elements->size() * 4; i++)
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
                    }
                }
            }
        }
        elements->at(i).has_solution=false;
    }

    model.optimize();

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
