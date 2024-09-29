//
// FinalGraph is a concise notation of graph designed for IO,
// which is more convenient to communicate with graph generator and visualizer written in Kotlin.
//
#include <cstdint>
#include <vector>

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