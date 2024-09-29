//
// IntermediateGraph is a graph with extra attributes to store information during transitive reduction computation
//

#ifndef ALGORITHMPROJECT_INTERMEDIATEGRAPH_H
#define ALGORITHMPROJECT_INTERMEDIATEGRAPH_H


#include <cstdint>
#include <map>
#include <vector>
#include <set>

class IntermediateNode;

class IntermediateEdge;


class IntermediateGraph {
public:
    std::vector<IntermediateNode*> startingNodes;
    std::vector<IntermediateNode*> nodes;
    std::vector<IntermediateEdge*> edges;
    // <intermediateEdge, whether it's added via In-Node> Just for verification purpose
    std::vector<std::pair<IntermediateEdge*, bool>> sortedEdgePairs;

    void markRedundantEdges_DFS();

    void markRedundantEdges_TROPlus(bool withVerification);

    void topoSort();


private:
    std::uint64_t current = 0;
    std::uint64_t numberOfIntervals = 1600;
    std::uint64_t numberOfHashValues = 160;
    std::map<IntermediateNode*, IntermediateNode*> gMap;
    std::set<IntermediateNode*> visitedInQuery;
    std::set<std::pair<IntermediateNode*, IntermediateNode*>> DFS_RI;

    void constructDFSRI();

    void dfsUtil(IntermediateNode* nodeA, IntermediateNode* nodeB);

    bool isReachable_BFL(IntermediateNode* a, IntermediateNode* b);

    void postOrderTraverse(IntermediateNode* node);

    std::uint64_t hash(IntermediateNode* node);

    void computeLabelOut(IntermediateNode* node);

    void computeLabelIn(IntermediateNode* node);

    bool isSubset(const std::unique_ptr<std::set<uint64_t>> &setA,
                  const std::unique_ptr<std::set<uint64_t>> &setB);

    void constructBFLRI();

    bool queryReachability_BFL(IntermediateNode* a, IntermediateNode* b);

    bool isRedundant_TROPlus(IntermediateEdge* edge);
};

class IntermediateEdge {
public:
    std::uint64_t id;
    IntermediateNode* startNode;
    IntermediateNode* endNode;

    bool isRedundant_DFS = false;
    bool isRedundant_TROPlus = false;
    bool isTopoTraversed = false;

    explicit IntermediateEdge(std::uint64_t id) : id(id) {}

};

class IntermediateNode {
public:
    std::uint64_t id;
    std::vector<IntermediateEdge*> incomingEdges;
    std::vector<IntermediateEdge*> outgoingEdges;

    std::uint64_t topoOrder = 0;
    bool isPostOrderAssigned = false;
    std::uint64_t discoverTime = 0;
    std::uint64_t finishTime = 0;
    std::unique_ptr<std::set<uint64_t>> LabelOut = nullptr;
    std::unique_ptr<std::set<uint64_t>> LabelIn = nullptr;

    explicit IntermediateNode(std::uint64_t id) : id(id) {}


};

/**
 * extra attributes for TRO-plus (in/out-degrees, is in/out-node)
 */
class IntermediateNodeWrapper {
public:
    IntermediateNode* node;
    uint64_t inDegree;
    uint64_t outDegree;
    bool isIn;

    IntermediateNodeWrapper(IntermediateNode* node, bool isIn) {
        this->node = node;
        if (isIn) {
            inDegree = node->incomingEdges.size();
            outDegree = 0;
            this->isIn = true;
        } else {
            outDegree = node->outgoingEdges.size();
            inDegree = 0;
            this->isIn = false;
        }
    }
};


#endif //ALGORITHMPROJECT_INTERMEDIATEGRAPH_H
