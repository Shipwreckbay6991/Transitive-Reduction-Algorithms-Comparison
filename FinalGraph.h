
/**
 * @file FinalGraph.h
 * @brief This file contains the definition of FinalGraph, FinalNode and FinalEdge,
 * which is a streamlined format of intermediateGraph/Node/Edge,
 * thus more convenient to communicate with graph generator and visualizer written as separate programs.
 */
#include <cstdint>
#include <vector>

/**
 *
 */
struct FinalNode {
    std::uint64_t id;
};

struct FinalEdge {
    std::uint64_t id;
    std::uint64_t startNodeId;
    std::uint64_t endNodeId;
};

struct FinalGraph {
    std::vector<FinalNode*> nodes;
    std::vector<FinalEdge*> edges;
};