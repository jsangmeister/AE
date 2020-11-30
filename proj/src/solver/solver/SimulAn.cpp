#include"SimulAn.hpp"
#include<iostream>
#include <stdlib.h>
#include <time.h> 
#include <math.h>

namespace labeler
{

int SimulAnSolver::simple_eval(std::vector<LabelElement>* elements, bool print_col)
{
    int labels_set = 0;
    int collisions = 0;
    for(auto i=(*elements).begin(); i!=(*elements).end(); ++i)
    {
        if (!i->has_solution) continue;
        ++labels_set;
        for(auto j=i+1; j!=(*elements).end(); ++j)
        {
            if (j->has_solution && this->collision(&*i, &*j)) {
                --collisions;
                if(print_col) std::cout << "Collision between " << i->label << " and " 
                << j->label << std::endl;
            }        
        }
    }
    return collisions !=0 ? collisions :labels_set;
}


void SimulAnSolver::permVector(std::vector<int>* vec)
{
    for(int i=vec->size()-1; i>=1; i--)
    {
        int j = rand() % (i+1);
        int tmp = (*vec)[i];
        (*vec)[i] = (*vec)[j];
        (*vec)[j] = tmp;
    }
}

void SimulAnSolver::addToStack(std::vector<std::pair<int, int>>* stack, LabelElement* el, int i)
{
    if (!el->has_solution)
    {
        stack->push_back(std::make_pair(i, -1));
    }
    else{
        int pos = 0;
        if (el->x != el->label_x1)
        {
            pos+=2;
        }
        if (el->y != el->label_y1)
        {
            pos+=1;
        }
        stack->push_back(std::make_pair(i, pos));
    }
}


void SimulAnSolver::intToPos(LabelElement& el, int pos)
{
    if(pos == -1)
    {
        el.has_solution = false;
        return;
    }
    el.has_solution = true;
    el.label_x1 = el.x - (pos/2) * el.width;
    el.label_y1 = el.y + (pos%2) * el.height;
}


int SimulAnSolver::solve(std::vector<LabelElement>* elements, std::vector<double> args, 
        int& init_active, int& max_active)
{   
    srand(time(NULL));
    //Prepare the conflict array
    std::vector<std::vector<int>> conflicts;
    for (auto i=elements->begin(); i!=elements->end(); ++i)
    {
        std::vector<int> tmp;
        conflicts.push_back(tmp);
    }
    //Dummy elements we use to test for conflicts
    LabelElement tmp_li, tmp_lj;
    tmp_li.has_solution = true;
    tmp_lj.has_solution = true;
    //We test every possible pair of labels for conflict and save possible conflicts for each pair
    for(int i=0; i<elements->size()-1; i++)
    {   
        //std::cout << "Conflict " << i << " of " << elements->size() << std::endl;
        tmp_li.width = (*elements)[i].width;
        tmp_li.height = (*elements)[i].height;
        tmp_li.x = (*elements)[i].x;
        tmp_li.y = (*elements)[i].y;
        for (int j=i+1; j<(*elements).size(); j++)
        {
            bool conf_found = false;
            tmp_lj.width = (*elements)[j].width;
            tmp_lj.height = (*elements)[j].height;
            tmp_lj.x = (*elements)[j].x;
            tmp_lj.y = (*elements)[j].y;
            for (int pos_li=0; pos_li<4; pos_li++)
            {
                intToPos(tmp_li, pos_li);
                for (int pos_lj=0; pos_lj<4; pos_lj++)
                {
                    intToPos(tmp_lj, pos_lj);
                    if(collision(&tmp_li, &tmp_lj))
                    {
                        conflicts[i].push_back(j);
                        conflicts[j].push_back(i);
                        conf_found = true;
                        break;
                    }
                }
                if (conf_found) break;
            }
        }
    }

    int active_labels = 0;

    bool randomize = true;
    if(args.size() == 3 && args[2] == 0)
    {
        randomize = false;
    }
    if (randomize)
    {
        //initialize random start
        for(int i=0; i<elements->size(); i++)
        {
            //std::cout << "Init " << i << " of " << elements->size() << std::endl;
            std::vector<int> positions{0, 1, 2, 3};
            permVector(&positions);
            int cols = 0;
            for (auto p: positions)
            {
                bool pos_ok = true;
                intToPos((*elements)[i], p);
                for (auto other_lab: conflicts[i])
                {
                    if (collision(&(*elements)[i], &(*elements)[other_lab]))
                    {
                        pos_ok = false;
                        cols++;
                        break;
                    }
                }
                if (pos_ok)
                {
                    active_labels++;
                    break;
                }
            }
            if (cols == 4)
            {
            (*elements)[i].has_solution=false; 
            }
        }
    }
    else
    {
        for (auto e : (*elements))
        {
            e.has_solution == false;
        }
    }
    
    //std::cout << active_labels << std::endl;
    init_active = active_labels;
    max_active = active_labels;

    //Start simulated annealing:
    double temperature = 1.0;
    double cold = 0.05;
    int max_tries = floor(sqrt((double)elements->size()));
    if (args.size() == 3)
    {
        temperature = args[0];
        max_tries = args[1];
    }
    const int max_changes = max_tries/2;
    while (temperature>cold)
    {
        int already_changed=0;
        for (int i=0; i<max_tries; i++)
        {
            //std::cout << "T=" << temperature << ", Try " << i << " of " << max_tries << std::endl;
            int last_active = active_labels;
            // Array-Stack to later undo if needed
            std::vector<std::pair<int, int>> undo_stack;
            //Choose random label
            int l = rand() % elements->size();
            addToStack(&undo_stack, &(*elements)[l], l);
            if (!(*elements)[l].has_solution)
            {
                active_labels++;
            }
            (*elements)[l].has_solution = true;
            //Find a new position
            while(true)
            {
                int new_pos = rand() % 4;
                int newx = (*elements)[l].x - (new_pos/2) * (*elements)[l].width;
                int newy = (*elements)[l].y + (new_pos%2) * (*elements)[l].height;
                if (newx != (*elements)[l].label_x1 || newy != (*elements)[l].label_y1)
                {
                    (*elements)[l].label_x1 = newx;
                    (*elements)[l].label_y1 = newy;
                    break;
                }
            }
            //Test all labels possibly directly affected
            for (auto check_label: conflicts[l])
            {
                //If the other label existed and no collision appeared, we can go on
                if ((*elements)[check_label].has_solution && !collision(&(*elements)[l], &(*elements)[check_label]))
                {
                    continue;
                }
                addToStack(&undo_stack, &(*elements)[check_label], check_label);

                //Try to find new place
                if (!(*elements)[check_label].has_solution)
                {
                    active_labels++;
                }
                std::vector<int> positions{0, 1, 2, 3};
                permVector(&positions);
                int cols = 0;
                for (auto p: positions)
                {
                    bool pos_ok = true;
                    intToPos((*elements)[check_label], p);
                    for (auto other_lab: conflicts[check_label])
                    {
                        if (collision(&(*elements)[check_label], &(*elements)[other_lab]))
                        {
                            pos_ok = false;
                            cols++;
                            break;
                        }
                    }
                    if (pos_ok)
                    {
                        break;
                    }
                }
                if (cols == 4)
                {
                    (*elements)[check_label].has_solution=false;
                    active_labels--;
                }
            }
            // Decide if to keep the new solution
            int dE = active_labels - last_active;
            double P = 1.0 - std::exp(dE/temperature);
            if (dE>0 || (rand()) / static_cast <float> (RAND_MAX)<=(1-P))
            {
                already_changed++;
                if (active_labels>max_active)
                {
                    max_active = active_labels;
                }
            }
            else
            {
                while (!undo_stack.empty())
                {
                    auto back = undo_stack.back();
                    intToPos((*elements)[back.first], back.second);
                    undo_stack.pop_back();
                }
                active_labels = last_active;
            }
            if (already_changed >= max_changes)
            {
                break;
            }
        }

        temperature = temperature * 0.9;
    }
    return active_labels;
}

}