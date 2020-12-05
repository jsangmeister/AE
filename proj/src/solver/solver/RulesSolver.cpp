#include <iostream>

#include "RulesSolver.hpp"

namespace labeler
{

std::vector<long unsigned int> RulesSolver::solve(std::vector<LabelElement>* elements, std::vector<double> args)
{
    this->labels_set = 0;
    std::cout << "Starting, got " << elements->size() << " elements" << std::endl;
    // setup conflict graph
    for (auto elem = elements->begin(); elem != elements->end(); ++elem) {
        elem->has_solution = false;
        auto ptr = &(*elem);
        std::vector<LabelCandidate> candidates;
        for (short p = 0; p < 4; ++p) {
            std::set<LabelCandidate*> conflicts;
            LabelCandidate cand = { ptr, p, conflicts };
            std::cout << "Created candidate: " << ptr << "; " << p << "; " << conflicts.size() << std::endl;
            candidates.push_back(cand);
        }
        std::cout << "Inserting " << elem->label << " into map with key " << ptr << std::endl;
        this->candidates_map.insert({ptr, candidates});
    }
    // fill conflict graph
    for (auto map_entry_i = this->candidates_map.begin(); map_entry_i != this->candidates_map.end(); ++map_entry_i) {
        for (auto c_i : map_entry_i->second) {
            auto [x_i, y_i] = this->getCandidatePosition(c_i);
            for (auto map_entry_j = std::next(map_entry_i); map_entry_j != this->candidates_map.end(); ++map_entry_j) {
                for (auto c_j : map_entry_j->second) {
                    auto [x_j, y_j] = this->getCandidatePosition(c_j);
                    // check collision
                    if (
                        (x_i - x_j - c_j.element->width) * (x_i + c_i.element->width - x_j) < 0
                        && (y_i - y_j + c_j.element->height)*(y_i - c_i.element->height - y_j) < 0
                    ) {
                        c_i.conflicts.insert(&c_j);
                        c_j.conflicts.insert(&c_i);
                    }
                }
            }
        }
    }

    // Phase 1: apply rules to all elements
    std::cout << "Phase 1" << std::endl;
    std::set<LabelElement*> set;
    for (auto map_entry : this->candidates_map) {
        set.insert(map_entry.first);
    }
    this->applyRules(&set);

    // Phase 2: remove troublemaker and apply rules exhaustively
    std::cout << "Phase 2" << std::endl;
    while (this->candidates_map.size() > 0) {
        long unsigned int max_candidates = 0, max_conflicts = 0;
        LabelCandidate* max = nullptr;
        for (auto map_entry : this->candidates_map) {
            if (map_entry.second.size() >= max_candidates) {
                if (map_entry.second.size() > max_candidates) {
                    max_candidates = map_entry.second.size();
                    max_conflicts = 0;
                    max = nullptr;
                }
                for (auto candidate : map_entry.second) {
                    if (candidate.conflicts.size() > max_conflicts) {
                        max_conflicts = candidate.conflicts.size();
                        max = &candidate;
                    }
                }
            }
        }
        if (max) {
            std::set<LabelElement*> neighbours;
            this->addNeighbourhood(&neighbours, *max);
            this->eliminateCandidate(*max);
            this->applyRules(&neighbours);
        }
    }

    int labels_set = 0;
    for (auto elem : *elements) {
        std::cout << elem.label << ": " << elem.has_solution << " (" << elem.label_x << ", " << elem.label_y << ")" << std::endl;
        if (elem.has_solution) {
            ++labels_set;
        }
    }
    std::cout << "Labels set: " << labels_set << std::endl;

    return std::vector<long unsigned int>{this->labels_set};
}

std::tuple<int, int> RulesSolver::getCandidatePosition(LabelCandidate c)
{
    return {
        c.element->x + c.element->width * (c.position % 2 - 1),
        c.element->y - c.element->height * (c.position / 2 - 1)
    };
}

void RulesSolver::applyRules(std::set<LabelElement*>* elements)
{
    std::cout << "Applying rules to " << elements->size() << " elements" << std::endl;
    while (!elements->empty()) {
        auto element_it = elements->begin();
        auto element = *element_it;
        elements->erase(element_it);

        if (element->has_solution) {
            continue;
        }
        std::cout << "Applying rules to label " << element->label << std::endl;

        auto candidates = this->candidates_map[element];
        if (candidates.size() == 0) {
            this->candidates_map.erase(element);
            continue;
        }

        for (auto candidate : candidates) {
            std::cout << "Checking candidate with position " << candidate.position << std::endl;
            // Rule L1
            // we can directly set candidates without conflicts
            if (candidate.conflicts.size() == 0) {
                std::cout << "L1" << std::endl;
                this->setAsSolution(candidate);
                this->addNeighbourhood(elements, candidate);
                this->eliminateOtherCandidates(candidate);
                break;
            }

            // Rule L2
            if (candidate.conflicts.size() == 1) {
                auto q_k = *candidate.conflicts.begin();
                auto q = q_k->element;
                for (auto q_j : this->candidates_map[q]) {
                    if (&q_j != q_k) {
                        if (q_j.conflicts.size() == 1) {
                            auto p_l = *q_j.conflicts.begin();
                            if (p_l != &candidate && p_l->element == element) {
                                std::cout << "L2" << std::endl;
                                this->setAsSolution(candidate);
                                this->setAsSolution(q_j);
                                this->addNeighbourhood(elements, candidate);
                                this->addNeighbourhood(elements, q_j);
                                this->eliminateOtherCandidates(candidate);
                                this->eliminateOtherCandidates(q_j);
                            }
                        }
                    }
                }
            }

            // Rule L3
            if (candidates.size() == 1) {
                bool is_clique = true;
                auto [x1, y1, x2, y2] = this->getRectangle(candidate);
                for (auto conflict : candidate.conflicts) {
                    auto [ox1, oy1, ox2, oy2] = this->getRectangle(*conflict);
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
                    for (auto conflict : candidate.conflicts) {
                        this->addNeighbourhood(elements, *conflict);
                        this->eliminateCandidate(*conflict);
                    }
                    this->candidates_map.erase(element);
                }
            }
        }
    }
}

// returns top left and bottom right coordinates
std::tuple<int, int, int, int> RulesSolver::getRectangle(LabelCandidate c)
{
    return {
        c.element->x,
        c.element->y,
        c.element->x + c.element->width,
        c.element->y + c.element->height,
    };
}

void RulesSolver::eliminateOtherCandidates(LabelCandidate c)
{
    auto candidates = this->candidates_map[c.element];
    for (auto other_candidate : candidates) {
        if (&other_candidate != &c) {
            this->eliminateCandidate(other_candidate);
        }
    }
    // remove element from candidates_map
    this->candidates_map.erase(c.element);
}

void RulesSolver::eliminateCandidate(LabelCandidate c)
{
    for (auto conflict : c.conflicts) {
        conflict->conflicts.erase(&c);
    }
}

void RulesSolver::setAsSolution(LabelCandidate c)
{
    std::cout << "Setting label " << c.element->label << std::endl;
    c.element->has_solution = true;
    std::tie(c.element->label_x, c.element->label_y) = this->getCandidatePosition(c);
    this->labels_set++;
}

void RulesSolver::addNeighbourhood(std::set<LabelElement*>* elements, LabelCandidate c)
{
    elements->insert(c.element);
    for (auto conflict : c.conflicts) {
        elements->insert(conflict->element);
    }
}

}
