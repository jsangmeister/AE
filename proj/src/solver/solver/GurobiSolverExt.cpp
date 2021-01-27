#include "GurobiSolverExt.hpp"
#include <algorithm>
#include <cstring>
#include "mcqd/mcqd.h"

namespace labeler
{

void CutCallback::callback()
{
    // call cutting plane heuristic on every solution
    if (where == GRB_CB_MIPSOL) {
        double* x = getSolution(m_vars, m_num_vars);
        std::cout << "ok" << std::endl;

        // sort indices by most conflicts
        std::sort(this->m_indices.begin(), this->m_indices.end(), [&](long unsigned int k, long unsigned int l) {
            return this->m_conflicts[k].size() > this->m_conflicts[l].size();
        });
        std::cout << this->m_indices.size() << std::endl;

        for (auto idx : this->m_indices) {
            auto conflicts = this->m_conflicts[idx];
            auto cnt = conflicts.size();
            bool** conn;
            conn = new bool*[cnt];
            for (size_t i=0; i < cnt; i++) {
                conn[i] = new bool[cnt];
                std::memset(conn[i], 1, cnt * sizeof(bool));
                std::memset(conn[i]+(i/4)*4, 0, 4 * sizeof(bool));
            }
            for (size_t i = 0; i < cnt; ++i) {
                auto c_i = conflicts.at(i);
                auto conflicts_of_ci = this->m_conflicts[c_i];
                for (size_t j = i + 1; j < cnt; ++j) {
                    auto c_j = conflicts.at(j);
                    bool connected = std::find(conflicts_of_ci.begin(), conflicts_of_ci.end(), c_j) != conflicts_of_ci.end();
                    conn[i][j] = conn[j][i] = connected;
                }
            }

            Maxclique md(conn, cnt, 0.025);
            int *qmax;
            int qsize;
            md.mcqdyn(qmax, qsize);
            std::cout << qsize << std::endl;

            double sum = 0;
            for (size_t q = 0; q < qsize; ++q) {
                sum += x[conflicts[qmax[q]]];
            }
            if (sum > 1) {
                GRBLinExpr cut = 0;
                for (size_t q = 0; q < qsize; ++q) {
                    auto clique_idx = conflicts[qmax[q]]; // the global index of this candidate
                    cut += m_vars[clique_idx];
                }
                addLazy(cut <= 1);
                break; // break after we found a violated cut to not iterate over and over again
            }
        }
        delete[] x;
    }
}

void GurobiSolverExt::intToPos(LabelElement& el, int pos)
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

std::vector<long unsigned int> GurobiSolverExt::solve(std::vector<LabelElement>* elements, std::vector<double> args)
{
    GRBEnv env = GRBEnv(true);
    //env.set("LogFile", "labeler_gurobi.log");
    env.set(GRB_IntParam_OutputFlag, 0);
    env.set("LazyConstraints", "1");
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

    // // add n label constraints
    // for (std::size_t i = 0; i<elements->size(); i++)
    // {
    //     GRBConstr loopLabelConstr = model.addConstr(variables[i*4] + variables[i*4+1] + variables[i*4+2] + variables[i*4+3] <= 1);
    //     labelConstr.push_back(loopLabelConstr);
    // }
    
    // // build conflict graph
    // for (auto &elem : *elements) {
    //     elem.has_solution = false;
    //     std::vector<LabelCandidate*> _candidates;
    //     for (short p = 0; p < 4; ++p) {
    //         std::set<LabelCandidate*> conflicts;
    //         LabelCandidate* cand = new LabelCandidate { &elem, p, conflicts };
    //         // std::cout << "Created candidate: " << cand->toString() << std::endl;
    //         _candidates.push_back(cand);
    //     }
    //     // std::cout << "Inserting " << elem.label << " into map with key " << &elem << std::endl;
    //     this->candidates.push_back(_candidates);
    // }
    // // fill conflict graph
    // for (auto entry_i = this->candidates.begin(); entry_i != this->candidates.end(); ++entry_i) {
    //     // std::cout << entry_i->first->label << std::endl;
    //     for (auto c_i : *entry_i) {
    //         auto [x_i, y_i] = this->getCandidatePosition(c_i);
    //         // std::cout << x_i << "; " << y_i << std::endl;
    //         for (auto entry_j = std::next(entry_i); entry_j != this->candidates.end(); ++entry_j) {
    //             for (auto c_j : *entry_j) {
    //                 auto [x_j, y_j] = this->getCandidatePosition(c_j);
    //                 // check collision
    //                 // std::cout << x_i << "," << y_i << "," << c_i->element->width << "," << c_i->element->height << "; " << x_j << "," << y_j << "," << c_j->element->width << "," << c_j->element->height << std::endl;
    //                 if (
    //                     (x_i - x_j - c_j->element->width) * (x_i + c_i->element->width - x_j) < 0
    //                     && (y_i - y_j + c_j->element->height) * (y_i - c_i->element->height - y_j) < 0
    //                 ) {
    //                     // std::cout << "adding conflict between " << c_i << " and " << c_j << " (" << c_i->element->label << " and " << c_j->element->label << ")" << std::endl;
    //                     c_i->conflicts.insert(c_j);
    //                     c_j->conflicts.insert(c_i);
    //                 }
    //             }
    //         }
    //     }
    // }
    // int zeros = 0;
    // for (auto entry : this->candidates) {
    //     bool has_zero = false;
    //     for (auto candidate : entry) {
    //         if (candidate->conflicts.size() == 0) {
    //             has_zero = true;
    //         }
    //     }
    //     if (has_zero) {
    //         zeros++;
    //     }
    // }
    // std::cout << zeros << "/" << this->candidates.size() << std::endl;

    std::vector<std::vector<size_t>> conflicts(elements->size() * 4, std::vector<size_t>());

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
                        conflicts[j * 4 + pos_f].push_back(i * 4 + pos_e);
                        conflicts[i * 4 + pos_e].push_back(j * 4 + pos_f);
                    }
                }
            }
        }
        elements->at(i).has_solution=false;
    }

    // return std::vector<long unsigned int>{0};

    // set callback
    CutCallback cb(variables, num_vars, conflicts);
    model.setCallback(&cb);

    try {
        model.optimize();
    } catch (GRBException e) {
        std::cout << "ERROR: " << e.getMessage() << "(" << e.getErrorCode() << ")" << std::endl;
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

std::tuple<int, int> GurobiSolverExt::getCandidatePosition(LabelCandidate* c)
{
    return {
        c->element->x + c->element->width * (c->position % 2 - 1),
        c->element->y - c->element->height * (c->position / 2 - 1)
    };
}

}
