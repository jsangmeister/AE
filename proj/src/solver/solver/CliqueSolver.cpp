#include "CliqueSolver.hpp"
#include <algorithm>
#include <cstring>

// #include "mcqd/mcqd.h"

namespace labeler
{

void CliqueSolver::intToPos(LabelElement& el, int pos)
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


int CliqueSolver::eval(std::vector<LabelElement>* elements, bool print_col)
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

std::vector<long unsigned int> CliqueSolver::solve(std::vector<LabelElement>* elements, std::vector<double> args)
{   
    /**
     * We reduce labeling to clique by building a "reversed" conflict graph.
     * Nodes connected are compatible labels and we search the biggest set of pairwise compatible labels
     * e.g. a clique in the reversed graph
     */


    // We initialize the no-conflict graph as complete graph with self loops, then remove selfloops 
    // and edges between labels of one node (those are alway incompatible)
    std::size_t num_vars = elements->size() * 4;
    bool** conn;
    conn = new bool*[num_vars];
    for (size_t i=0; i < num_vars; i++) {
        conn[i] = new bool[num_vars];
        std::memset(conn[i], 1, num_vars * sizeof(bool));
        std::memset(conn[i]+(i/4)*4, 0, 4 * sizeof(bool));
        conn[i][i] = 0;
    }

    // Find all conflicts and remove edges in no-conflict graph
    std::size_t range4[] = {0, 1, 2, 3}; 
    for (std::size_t i = 0; i<elements->size(); i++)
    {
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
                        conn[i*4+pos_e][j*4+pos_f] = 0;
                        conn[j*4+pos_f][i*4+pos_e] = 0;
                    }
                }
            }
        }
        elements->at(i).has_solution=false;
    }

    //start mcqd clique algorithm
    // Maxclique md(conn, num_vars, 0.025);
    int *qmax;
    int qsize;
    // md.mcqdyn(qmax, qsize);


    for (int i=0; i<qsize; i++)
    {
        intToPos(elements->at(qmax[i]/4), qmax[i]%4);
    }

    std::size_t active_label = qsize;

    return std::vector<long unsigned int>{active_label};
}

}
