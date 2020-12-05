#ifndef SRC_RULES_SOLVER_HPP_
#define SRC_RULES_SOLVER_HPP_

#include "../Solver.hpp"
#include <set>
#include <map>

namespace labeler
{

struct LabelCandidate {
    LabelElement* element;
    // possible positions:
    // 0   1
    //   x
    // 2   3
    short position;
    std::set<LabelCandidate*> conflicts;
};

class RulesSolver : public Solver
{
public:
    std::vector<long unsigned int> solve(std::vector<LabelElement>* elements, std::vector<double> args);

private:
    std::tuple<int, int> getCandidatePosition(LabelCandidate c);

    void applyRules(std::set<LabelElement*>* elements);

    std::tuple<int, int, int, int> getRectangle(LabelCandidate c);

    void eliminateOtherCandidates(LabelCandidate c);

    void eliminateCandidate(LabelCandidate c);

    void setAsSolution(LabelCandidate c);

    void addNeighbourhood(std::set<LabelElement*>* elements, LabelCandidate c);

    std::map<LabelElement*, std::vector<LabelCandidate>> candidates_map;

    long unsigned int labels_set = 0;
};

}

#endif
