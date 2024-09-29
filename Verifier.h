
#ifndef ALGORITHMPROJECT_VERIFIER_H
#define ALGORITHMPROJECT_VERIFIER_H


#include "IntermediateGraph.h"

class Verifier {
public:
    static bool crossCheckTRCorrectness(std::string fileName);

    static bool verifyGraphTopoOrder(std::string fileName);

    static bool verifyNodeTopoOrder(IntermediateNode* node);

    static bool verifyEdgesSortingOrder(std::string fileName);
};


#endif //ALGORITHMPROJECT_VERIFIER_H
