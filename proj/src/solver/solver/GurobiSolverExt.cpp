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
        std::cout << "start" << std::endl;
        for (int i = 0; i < m_num_vars; i++) {
            std::cout << x[i] << "  ";
        }
        std::cout << std::endl;

        for (auto [constr, indices] : m_constr) {
            double sum = 0;
            for (auto i : indices) {
                sum += x[i];
            }
            if (sum > 1) {
                addLazy(constr);
            }
        }

        int found = 0, added = 0;
        auto _it = m_indices.begin();
        while (_it != m_indices.end()) {
            auto it = _it++;
            auto idx = *it;
            // std::cout << idx << std::endl;
            // continue;
            auto conflicts = this->m_conflicts[idx];
            for (auto c : conflicts) {
                std::cout << c << "  ";
            }
            std::cout << std::endl;
            auto cnt = conflicts.size();
            std::cout << idx << "; " << cnt << std::endl;
            // std::cout << idx << "; " << cnt << "    " << m_vars[idx].get(GRB_DoubleAttr_LB) << "|" << m_vars[idx].get(GRB_DoubleAttr_UB) << std::endl;
            // if candidate is already fixed, we can skip this
            // if (m_vars[idx].get(GRB_DoubleAttr_UB) == m_vars[idx].get(GRB_DoubleAttr_LB)) {
            //     std::cout << "already fixed" << std::endl;
            //     m_indices.erase(it--);
            //     continue;
            // }
            // fixiate candidates without conflicts
            if (cnt == 0) {
                // std::cout << "fixing now" << std::endl;
                // for (int i = 0; i < 4; i++) {
                //     if (i != idx % 4) {
                //         m_vars[(idx / 4) * 4 + i].set(GRB_DoubleAttr_UB, 0);
                //         m_vars[(idx / 4) * 4 + i].set(GRB_DoubleAttr_LB, 0);
                //     } else {
                //         m_vars[idx].set(GRB_DoubleAttr_UB, 1);
                //         m_vars[idx].set(GRB_DoubleAttr_LB, 1);
                //     }
                // }
                m_indices.erase(it--);
                continue;
            }
            if (cnt == 1 && added > 0) {
                break;
            }
            bool** conn;
            conn = new bool*[cnt + 1];
            for (size_t i = 0; i < cnt; i++) {
                conn[i] = new bool[cnt + 1];
                conn[i][cnt] = true;
            }
            conn[cnt] = new bool[cnt + 1];
            std::memset(conn[cnt], true, (cnt + 1) * sizeof(bool));

            for (size_t i = 0; i < cnt; ++i) {
                auto c_i = conflicts[i];
                auto conflicts_of_ci = this->m_conflicts[c_i];
                for (size_t j = i + 1; j < cnt; ++j) {
                    auto c_j = conflicts[j];
                    bool connected = std::find(conflicts_of_ci.begin(), conflicts_of_ci.end(), c_j) != conflicts_of_ci.end();
                    conn[i][j] = conn[j][i] = connected;
                }
                conn[i][i] = 1;
            }
            std::cout << "Conn matrix:" << std::endl;
            for (int i = 0; i < cnt + 1; i++) {
                for (int j = 0; j < cnt + 1; j++) {
                    std::cout << conn[i][j] << " ";
                }
                std::cout << std::endl;
            }

            Maxclique md(conn, cnt + 1, 0.005);
            int *qmax;
            int qsize;
            md.mcqdyn(qmax, qsize);
            for (int i = 0; i < cnt + 1; i++) {
                delete[] conn[i];
            }
            delete[] conn;
            std::cout << "qsize: " << qsize << std::endl;
            std::cout << "qmax: ";
            for (int i = 0; i < qsize; i++) {
                std::cout << qmax[i] << " ";
            }
            std::cout << std::endl;
            if (qsize < 2) {
                continue;
            }

            // bool** sconn;
            // sconn = new bool*[qsize];
            // for (size_t i = 0; i < qsize; i++) {
            //     sconn[i] = new bool[qsize];
            // }
            // for (size_t i = 0; i < qsize; ++i) {
            //     for (size_t j = 0; j < qsize; ++j) {
            //         sconn[i][j] = conn[qmax[i]][qmax[j]];
            //     }
            // }
            // std::cout << "SConn matrix:" << std::endl;
            // for (int i = 0; i < qsize; i++) {
            //     for (int j = 0; j < qsize; j++) {
            //         std::cout << sconn[i][j] << " ";
            //     }
            //     std::cout << std::endl;
            // }

            std::cout << "cut indices: ";
            double sum = 0;
            GRBLinExpr cut = 0;
            std::vector<size_t> indices(qsize);
            for (size_t q = 0; q < qsize; ++q) {
                auto clique_idx = qmax[q] == cnt ? idx : conflicts[qmax[q]]; // the global index of this candidate
                std::cout << clique_idx << "  ";
                indices[q] = clique_idx;
                sum += x[clique_idx];
                cut += this->m_vars[clique_idx];
            }
            delete[] qmax;
            std::cout << std::endl;
            std::cout << "sum: " << sum << std::endl;
            
            if (sum > 1) {
                auto constr = cut <= 1;
                addLazy(constr);
                m_constr.push_back(std::make_pair(constr, indices));
                // remove conflicts from array to prevent adding cuts multiple times
                for (int gi : indices) {
                    for (int gj : indices) {
                        if (gi != gj) {
                            auto it = std::find(m_conflicts[gi].begin(), m_conflicts[gi].end(), gj);
                            if (it == m_conflicts[gi].end()) {
                                std::cout << "ERROR. gi = " << gi << "   gj = " << gj << std::endl;
                                std::copy(indices.begin(), indices.end(), std::ostream_iterator<int>(std::cout, ","));
                                std::cout << std::endl;
                                std::copy(m_conflicts[gi].begin(), m_conflicts[gi].end(), std::ostream_iterator<int>(std::cout, ","));
                                std::cout << std::endl;
                            }
                            m_conflicts[gi].erase(it);
                        }
                    }
                }
                added++;
                if (sum > 1) {
                    found++;
                    // if (found > 0) {
                    //     std::cout << "finished" << std::endl;
                    //     break;
                    // }
                }
            }
        }
        std::cout << "found: " << found << ";  added: " << added << std::endl;
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
    env.start();
    GRBModel model = GRBModel(env);
    model.set(GRB_IntParam_LazyConstraints, 1);

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
    std::cout << elements->size() << std::endl;

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
