#include <iostream>

#include "RulesSolver.hpp"
#include <numeric>

namespace labeler
{

void RulesSolver::print_map() {
    for (auto &map_entry : this->candidates_map) {
        std::cout << map_entry.first->label << std::endl;
        for (auto &candidate : map_entry.second) {
            std::cout << candidate->toString() << ": ";
            for (auto &conflict : candidate->conflicts) {
                std::cout << conflict->toString() + " ";
            }
            std::cout << std::endl;
        }
    }
}

std::vector<long unsigned int> RulesSolver::solve(std::vector<LabelElement>* elements, std::vector<double> args)
{
    this->labels_set = 0;
    // std::cout << "Starting, got " << elements->size() << " elements" << std::endl;
    // setup conflict graph
    for (auto &elem : *elements) {
        elem.has_solution = false;
        std::vector<LabelCandidate*> candidates;
        for (short p = 0; p < 4; ++p) {
            std::set<LabelCandidate*> conflicts;
            LabelCandidate* cand = new LabelCandidate { &elem, p, conflicts };
            // std::cout << "Created candidate: " << cand->toString() << std::endl;
            candidates.push_back(cand);
        }
        // std::cout << "Inserting " << elem.label << " into map with key " << &elem << std::endl;
        this->candidates_map.insert({&elem, candidates});
    }
    // this->print_map();
    // fill conflict graph
    for (auto map_entry_i = this->candidates_map.begin(); map_entry_i != this->candidates_map.end(); ++map_entry_i) {
        // std::cout << map_entry_i->first->label << std::endl;
        for (auto c_i : map_entry_i->second) {
            auto [x_i, y_i] = this->getCandidatePosition(c_i);
            // std::cout << x_i << "; " << y_i << std::endl;
            for (auto map_entry_j = std::next(map_entry_i); map_entry_j != this->candidates_map.end(); ++map_entry_j) {
                for (auto c_j : map_entry_j->second) {
                    auto [x_j, y_j] = this->getCandidatePosition(c_j);
                    // check collision
                    // std::cout << x_i << "," << y_i << "," << c_i->element->width << "," << c_i->element->height << "; " << x_j << "," << y_j << "," << c_j->element->width << "," << c_j->element->height << std::endl;
                    if (
                        (x_i - x_j - c_j->element->width) * (x_i + c_i->element->width - x_j) < 0
                        && (y_i - y_j + c_j->element->height) * (y_i - c_i->element->height - y_j) < 0
                    ) {
                        // std::cout << "adding conflict between " << c_i << " and " << c_j << " (" << c_i->element->label << " and " << c_j->element->label << ")" << std::endl;
                        c_i->conflicts.insert(c_j);
                        c_j->conflicts.insert(c_i);
                    }
                }
            }
        }
    }
    int zeros = 0;
    for (auto &map_entry : this->candidates_map) {
        std::cout << map_entry.first->label << std::endl;
        bool has_zero = false;
        for (auto &candidate : map_entry.second) {
            std::cout << candidate->toString() << ": " << candidate->conflicts.size() << std::endl;
            if (candidate->conflicts.size() == 0) {
                has_zero = true;
            }
        }
        if (has_zero) {
            zeros++;
        }
    }
    std::cout << zeros << "/" << this->candidates_map.size() << std::endl;
    // this->print_map();

    // Phase 1: apply rules to all elements
    std::set<LabelElement*> set;
    for (auto &map_entry : this->candidates_map) {
        set.insert(map_entry.first);
        // int sum = std::accumulate(map_entry.second.begin(), map_entry.second.end(), 0, [](int i, const LabelCandidate* o){ return o->conflicts.size() + i; });
        // std::cout << "Label " << map_entry.first->label << " has " << sum << " conflicts" << std::endl;
    }
    // std::cout << "Phase 1" << std::endl;
    this->applyRules(&set);
    return std::vector<long unsigned int>{0};

    // Phase 2: remove troublemaker and apply rules exhaustively
    // std::cout << std::endl << std::endl << "Phase 2" << std::endl;
    while (this->candidates_map.size() > 0) {
        // this->print_map();
        long unsigned int max_candidates = 0, max_conflicts = 0;
        LabelCandidate* max = nullptr;

        auto map_entry = this->candidates_map.begin();
        while (map_entry != this->candidates_map.end()) {
            // std::cout << map_entry->first->label << ": " << map_entry->second.size() << " candidates" << std::endl;
            if (map_entry->second.size() > 0) {
                if (map_entry->second.size() >= max_candidates) {
                    for (auto &candidate : map_entry->second) {
                        if (map_entry->second.size() > max_candidates || candidate->conflicts.size() > max_conflicts) {
                            max_candidates = map_entry->second.size();
                            max_conflicts = candidate->conflicts.size();
                            max = candidate;
                        }
                    }
                }
                ++map_entry;
            } else {
                map_entry = this->candidates_map.erase(map_entry);
            }
        }
        // this->print_map();
        if (max) {
            // std::cout << "Removing candidate " << max->position << " of label " << max->element->label << std::endl;
            std::set<LabelElement*> neighbours;
            this->addNeighbourhood(&neighbours, max);
            this->eliminateCandidate(max);
            this->applyRules(&neighbours);
        } else {
            // std::cout << "No max found! max_cand: " << max_candidates << "; max_conflicts: " << max_conflicts << std::endl;
        }
    }

    return std::vector<long unsigned int>{this->labels_set};
}

std::tuple<int, int> RulesSolver::getCandidatePosition(LabelCandidate* c)
{
    return {
        c->element->x + c->element->width * (c->position % 2 - 1),
        c->element->y - c->element->height * (c->position / 2 - 1)
    };
}

void RulesSolver::applyRules(std::set<LabelElement*>* elements)
{
    // std::cout << "Applying rules to " << elements->size() << " elements" << std::endl;
    while (!elements->empty()) {
        auto element_it = elements->begin();
        auto element = *element_it;
        elements->erase(element_it);
        // std::cout << "Got label " << element->label << std::endl;

        if (element->has_solution) {
            continue;
        }
        // this->print_map();
        // std::cout << "Applying rules to label " << element->label << std::endl;

        auto candidates = this->candidates_map[element];
        for (auto &candidate : candidates) {
            // std::cout << "Checking candidate with position " << candidate->position << std::endl;
            // std::cout << "(" << candidate->conflicts.size() << " conflicts)" << std::endl;
            // Rule L1
            // we can directly set candidates without conflicts
            if (candidate->conflicts.size() == 0) {
                std::cout << "L1" << std::endl;
                this->setAsSolution(candidate);
                // this->print_map();
                this->eliminateOtherCandidates(candidate);
                break;
            }

            // Rule L2
            if (candidate->conflicts.size() == 1) {
                auto q_k = *candidate->conflicts.begin();
                auto q = q_k->element;
                for (auto &q_j : this->candidates_map[q]) {
                    if (q_j != q_k) {
                        if (q_j->conflicts.size() == 1) {
                            auto p_l = *q_j->conflicts.begin();
                            if (p_l != candidate && p_l->element == element) {
                                std::cout << "L2" << std::endl;
                                this->setAsSolution(candidate);
                                this->setAsSolution(q_j);
                                this->addNeighbourhood(elements, candidate);
                                this->addNeighbourhood(elements, q_j);
                                this->eliminateOtherCandidates(candidate);
                                this->eliminateOtherCandidates(q_j);
                                break;
                            }
                        }
                    }
                }
            }

            // Rule L3
            if (candidates.size() == 1) {
                bool is_clique = true;
                auto [x1, y1, x2, y2] = this->getRectangle(candidate);
                for (auto &conflict : candidate->conflicts) {
                    auto [ox1, oy1, ox2, oy2] = this->getRectangle(conflict);
                    x1 = std::max(x1, ox1);
                    y1 = std::min(y1, oy1);
                    x2 = std::min(x2, ox2);
                    y2 = std::max(y2, oy2);
                    if (x1 >= x2 || y1 >= y2) {
                        // degenerated rectangle -> no clique
                        is_clique = false;
                        break;
                    }
                }
                if (is_clique) {
                    std::cout << "L3" << std::endl;
                    this->setAsSolution(candidate);
                    this->addNeighbourhood(elements, candidate);
                    std::set<LabelCandidate*> copy;
                    for (auto &conflict : candidate->conflicts) {
                        this->addNeighbourhood(elements, conflict);
                        copy.insert(conflict);
                    }
                    for (auto &conflict : copy) {
                        this->eliminateCandidate(conflict);
                    }
                    this->candidates_map.erase(element);
                }
            }
        }
    }
}

// returns top left and bottom right coordinates
std::tuple<int, int, int, int> RulesSolver::getRectangle(LabelCandidate* c)
{
    return {
        c->element->x,
        c->element->y,
        c->element->x + c->element->width,
        c->element->y + c->element->height,
    };
}

void RulesSolver::eliminateOtherCandidates(LabelCandidate* c)
{
    // std::cout << "erasing other candidates" << std::endl;
    auto candidates = this->candidates_map[c->element];
    std::vector<LabelCandidate*> to_delete;
    for (auto &other_candidate : candidates) {
        if (other_candidate != c) {
            to_delete.push_back(other_candidate);
        }
    }
    for (auto &td : to_delete) {
        this->eliminateCandidate(td);
    }
    // remove element from candidates_map
    this->candidates_map.erase(c->element);
    // std::cout<<"finish"<<std::endl;
}

void RulesSolver::eliminateCandidate(LabelCandidate* c)
{
    // this->print_map();
    // std::cout << "Deleting candidate " << c->toString() << " with " << c->conflicts.size() << " conflicts" << std::endl;
    for (auto conflict : c->conflicts) {
        // std::cout << "deleting from " << conflict->toString() << std::endl;
        conflict->conflicts.erase(c);
    }
    // this->print_map();
    auto &candidates = this->candidates_map[c->element];
    auto it = std::find(candidates.begin(), candidates.end(), c);
    if (it != candidates.end()) {
        candidates.erase(it);
    }
    // this->print_map();
}

void RulesSolver::setAsSolution(LabelCandidate* c)
{
    // std::cout << "Setting label " << c->element->label << std::endl;
    c->element->has_solution = true;
    std::tie(c->element->label_x, c->element->label_y) = this->getCandidatePosition(c);
    this->labels_set++;
}

void RulesSolver::addNeighbourhood(std::set<LabelElement*>* elements, LabelCandidate* c)
{
    elements->insert(c->element);
    // std::cout << "Conflicts count for candidate " << c->toString() << ": " << c->conflicts.size() << std::endl;
    for (auto conflict = c->conflicts.begin(); conflict != c->conflicts.end(); ++conflict) {
        // std::cout << "adding conflict with " << *conflict << std::endl;
        // std::cout << "(" << (*conflict)->toString() << ")" << std::endl;
        elements->insert((*conflict)->element);
    }
}

}
