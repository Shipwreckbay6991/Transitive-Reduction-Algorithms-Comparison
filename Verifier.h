
/**
 * @file Verifier.h
 * @brief This file contains the verifier for transitive reduction  algorithms and their sub-algorithms
 */
#ifndef ALGORITHMPROJECT_VERIFIER_H
#define ALGORITHMPROJECT_VERIFIER_H


#include "IntermediateGraph.h"

class Verifier {
public:
    /**
     * @brief cross check the correctness of two different transitive reduction algorithms by comparing their result
     */
    static bool crossCheckTRCorrectness(std::string fileName);

    /**
     * @brief verify whether topological order of the given graph is correct
     */
    static bool verifyGraphTopoOrder(std::string fileName);

    /**
     * @brief a recursive helper for function verifyGraphTopoOrder
     */
    static bool verifyNodeTopoOrder(IntermediateNode* node);

    /**
     * @brief verify whether edges in TRO_Plus algorithm is sorted correctly before starting redundancy check
     */
    static bool verifyEdgesSortingOrder(std::string fileName);
};


#endif //ALGORITHMPROJECT_VERIFIER_H
