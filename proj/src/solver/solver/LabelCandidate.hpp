#ifndef SRC_LABEL_CANDIDATE_HPP_
#define SRC_LABEL_CANDIDATE_HPP_

#include <set>

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
    long unsigned int idx = 0;

    inline std::string toString() const {
        return this->element->label + "_" + std::to_string(this->position);
    }

    inline bool operator==(const LabelCandidate& other) const {
        return this->element == other.element && this->position == other.position;
    }
};

}

#endif
