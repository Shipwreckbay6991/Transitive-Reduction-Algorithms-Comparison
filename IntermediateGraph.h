//
//
//
/**
 * @file IntermediateGraph.h
 * @brief IntermediateGraph is a graph with extra attributes to store information during transitive reduction computation
 */
#ifndef ALGORITHMPROJECT_INTERMEDIATEGRAPH_H
#define ALGORITHMPROJECT_INTERMEDIATEGRAPH_H


#include <cstdint>
#include <map>
#include <vector>
#include <set>
#include <unordered_set>


class IntermediateNode;

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
    std::unique_ptr<std::unordered_set<uint64_t>> LabelOut = nullptr;
    std::unique_ptr<std::unordered_set<uint64_t>> LabelIn = nullptr;

    explicit IntermediateNode(std::uint64_t id) : id(id) {}


};

/**
 * @brief hashIntermediateNode, PairHash, PairEqual is used to hash pair of hashIntermediateNode*,
 * so that it can be stored in std::unordered_set without any problems
 */
std::uint64_t hashIntermediateNode(IntermediateNode* node);

struct PairHash {
    std::size_t operator()(const std::pair<IntermediateNode*, IntermediateNode*> &p) const {
        // Assuming your hash function for IntermediateNode is called hash_intermediate_node
        std::size_t h1 = hashIntermediateNode(p.first);
        std::size_t h2 = hashIntermediateNode(p.second);
        return h1 ^ (h2 << 1);
    }
};

struct PairEqual {
    bool operator()(const std::pair<IntermediateNode*, IntermediateNode*> &lhs,
                    const std::pair<IntermediateNode*, IntermediateNode*> &rhs) const {
        return lhs.first == rhs.first && lhs.second == rhs.second;
    }
};


class IntermediateGraph {
public:
    std::vector<IntermediateNode*> startingNodes;
    std::vector<IntermediateNode*> nodes;
    std::vector<IntermediateEdge*> edges;
    // <intermediateEdge, whether it's added via In-Node> Just for verification purpose
    std::vector<std::pair<IntermediateEdge*, bool>> sortedEdgePairs;

    /**
     * @brief mark the redundant edges in graph by setting edge attribute isRedundant_DFS to true.
     * The DFS_RI is used by default, can be switched to BFL_RI in implementation.
     */
    void markRedundantEdges_DFS();

    /**
     * @brief mark the redundant edges in graph by setting edge attribute isRedundant_TROPlus to true.
     * The BFL_RI is used by default, can be switched to DFS_RI in implementation.
     */
    void markRedundantEdges_TROPlus(bool withVerification);

    /**
     * @brief construct DFS_RI for reachability query
     */
    void constructDFSRI();

    /**
     * @brief construct BFL_RI for reachability query
     */
    void constructBFLRI();

    ~IntermediateGraph() {
        for (auto node: nodes) {
            delete node;
        }
        startingNodes.clear();
        nodes.clear();
        for (auto edge: edges) {
            delete edge;
        }
        edges.clear();
        sortedEdgePairs.clear();
        gMap.clear();
        visitedInQuery.clear();
        DFS_RI.clear();
    }

private:
    std::uint64_t current = 0;
    std::uint64_t numberOfIntervals = 1600;
    std::uint64_t numberOfHashValues = 160;
    std::map<IntermediateNode*, IntermediateNode*> gMap;
    std::unordered_set<IntermediateNode*> visitedInQuery;
    std::unordered_set<std::pair<IntermediateNode*, IntermediateNode*>, PairHash, PairEqual> DFS_RI;

    /**
     * @brief assign topological order to all nodes in graph. Order stored in node attribute topoOrder
     */
    void topoSort();

    /**
     * @brief recursive helper of function constructDFSRI
     */
    void dfsUtil(IntermediateNode* nodeA, IntermediateNode* nodeB);

    /**
     * @brief recursive helper of function queryReachability_BFL
     */
    bool isReachable_BFL(IntermediateNode* a, IntermediateNode* b);

    /**
     * @brief traverse tree with the given node as root, store discover time and finish time in each visited node
     */
    void postOrderTraverse(IntermediateNode* node);

    /**
     * @brief hash a given node. The return hash value is in range of [1, intermediateGraph::numberOfHashValues]
     */
    std::uint64_t hash(IntermediateNode* node);

    /**
     * @brief compute the BFL label of all the groups that the given node can reach
     */
    void computeLabelOut(IntermediateNode* node);

    /**
     * @brief compute the BFL label of all the groups that can reach the given node
     */
    void computeLabelIn(IntermediateNode* node);

    /**
     * @brief check whether setA is subset of setB
     */
    bool isSubset(const std::unique_ptr<std::unordered_set<uint64_t>> &setA,
                  const std::unique_ptr<std::unordered_set<uint64_t>> &setB);

    /**
     * @brief check whether a can reaches b.
     */
    bool queryReachability_BFL(IntermediateNode* a, IntermediateNode* b);

    /**
     * @brief check if the edge is redundant and mark result by changing its attribute isRedundant_TROPlus
     */
    bool isRedundant_TROPlus(IntermediateEdge* edge);
};


/**
 * a class contains extra attributes for edge sorting in TRO-plus algorithm (in/out-degrees, is in/out-node).
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
