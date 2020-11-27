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

void permVector(std::vector<int>* vec)
{
    for(int i=vec->size()-1; i>=1; i--)
    {
        int j = rand() % (i+1);
        int tmp = (*vec)[i];
        (*vec)[i] = (*vec)[j];
        (*vec)[j] = tmp;
    }
}

void addToStack(std::vector<std::pair<int, int>>* stack, LabelElement* el, int i)
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


int SimulAnSolver::solve(std::vector<LabelElement>* elements)
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
                tmp_li.label_x1 = tmp_li.x - (pos_li/2) * tmp_li.width;
                tmp_li.label_y1 = tmp_li.y + (pos_li%2) *tmp_li.height;
                for (int pos_lj=0; pos_lj<4; pos_lj++)
                {
                    tmp_lj.label_x1 = tmp_lj.x - (pos_lj/2) * tmp_lj.width;
                    tmp_lj.label_y1 = tmp_lj.y + (pos_lj%2) * tmp_lj.height;
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
    /*
    for (int i=0; i<(*elements).size(); i++)
    {
        std::cout << i+1 << "   " << (*elements)[i].label << std::endl;
        for (auto e=conflicts[i].begin(); e!=conflicts[i].end(); ++e)
        {
            std::cout << (*e) + 1 << "  ";
        }
        std::cout << std::endl;
    }*/

    int active_labels = 0;
    //initialize random start
    for(int i=0; i<elements->size(); i++)
    {
        (*elements)[i].has_solution=true;
        std::vector<int> positions{0, 1, 2, 3};
        permVector(&positions);
        int cols = 0;
        for (auto p: positions)
        {
            bool pos_ok = true;
            (*elements)[i].label_x1 = (*elements)[i].x - (p/2) * (*elements)[i].width;
            (*elements)[i].label_y1 = (*elements)[i].y + (p%2) *(*elements)[i].height;
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
    std::cout << active_labels << std::endl;

    //Start simulated annealing:
    double temperature = 0.91;//0.91; //Leads to P~=0.66 by dif 1
    const double cold = 0.05;//0.05; //with a decrease of 10%, this amounts to 30 loop
    const int max_tries = floor(sqrt((double)elements->size())); //change to floor(sqrt(n))
    const int max_changes = max_tries/2;
    while (temperature>cold)
    {
        int already_changed=0;
        for (int i=0; i<max_tries; i++)
        {
            int last_active = active_labels;
            /*
            std::cout << "active before try: " << active_labels << std::endl;
            int count = 0;
            for (auto c: *elements)
            {
                if (c.has_solution)
                count++;
            }
            std::cout << "Really active before try: " << count << std::endl;
            */
            // Array-Stack to later undo if needed
            std::vector<std::pair<int, int>> undo_stack;
            //Choose random label
            int l = rand() % elements->size();
            //std::cout << "Chosen: " << l << std::endl;
            //std::cout << "Added to Stack " << l << " was " << (*elements)[l].has_solution << std::endl;
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
                //std::cout << l << " causes check in " << check_label << std::endl;
                //If the other label existed and no collision appeared, we can go on
                if ((*elements)[check_label].has_solution && !collision(&(*elements)[l], &(*elements)[check_label]))
                {
                    continue;
                }
                //std::cout << "Added to Stack " << check_label << " was " << (*elements)[check_label].has_solution << std::endl;
                addToStack(&undo_stack, &(*elements)[check_label], check_label);

                //Try to find new place
                if (!(*elements)[check_label].has_solution)
                {
                    active_labels++;
                }
                (*elements)[check_label].has_solution = true;
                std::vector<int> positions{0, 1, 2, 3};
                permVector(&positions);
                int cols = 0;
                for (auto p: positions)
                {
                    bool pos_ok = true;
                    (*elements)[check_label].label_x1 = (*elements)[check_label].x - (p/2) * (*elements)[check_label].width;
                    (*elements)[check_label].label_y1 = (*elements)[check_label].y + (p%2) *(*elements)[check_label].height;
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
            //simple_eval(elements, true);
            // Decide if to keep the new solution
            int dE = active_labels - last_active;
            double P = 1.0 - std::exp(-dE/temperature);

            //std::cout << P << std::endl;
            if ((rand()) / static_cast <float> (RAND_MAX)<=P)
            {
                already_changed++;
            }
            else
            {

                /*std::cout << "active before stackclean: " << active_labels << std::endl;
                int count = 0;
                for (auto c: *elements)
                {
                    if (c.has_solution)
                    count++;
                }
                std::cout << "Really active before stackclean: " << count << std::endl;
                std::cout << "Stacksize and active before cleaning " << undo_stack.size() << "   " << active_labels << std::endl;*/
                while (!undo_stack.empty())
                {
                    auto back = undo_stack.back();
                    //std::cout << "Popped " << back.first << ", is " << back.second << std::endl;
                    switch (back.second)
                    {
                    case -1:
                        (*elements)[back.first].has_solution = false;
                        break;
                    case 1:
                        (*elements)[back.first].has_solution = true;
                        (*elements)[back.first].label_x1 = (*elements)[back.first].x;
                        (*elements)[back.first].label_y1 = (*elements)[back.first].y + (*elements)[back.first].height;
                        break;
                    case 3:
                        (*elements)[back.first].has_solution = true;
                        (*elements)[back.first].label_x1 = (*elements)[back.first].x - (*elements)[back.first].width;
                        (*elements)[back.first].label_y1 = (*elements)[back.first].y + (*elements)[back.first].height;
                        break;
                    case 2:
                        (*elements)[back.first].has_solution = true;
                        (*elements)[back.first].label_x1 = (*elements)[back.first].x - (*elements)[back.first].width;
                        (*elements)[back.first].label_y1 = (*elements)[back.first].y;
                        break;
                    case 0:
                        (*elements)[back.first].has_solution = true;
                        (*elements)[back.first].label_x1 = (*elements)[back.first].x;
                        (*elements)[back.first].label_y1 = (*elements)[back.first].y;
                        break;
                    default:
                        break;
                    }
                    undo_stack.pop_back();
                }
                active_labels = last_active;
            }
            //std::cout << active_labels << std::endl;
            if (already_changed >= max_changes)
            {
                break;
            }
        }

        temperature = temperature * 0.9;
    }

    //simple_eval(elements, true);
    std::cout << active_labels << std::endl;
    return active_labels;
}

}