//
// IntermediateGraph is a graph with extra attributes to store information during transitive reduction computation
//

#include "IntermediateGraph.h"
#include <iostream>
#include <cmath>
#include <memory>
#include <algorithm>


/**
 * DFS Reachability Index construction
 */
void IntermediateGraph::constructDFSRI() {
    for (IntermediateNode* node: nodes) {
        dfsUtil(node, node);
    }
}

/**
 * DFS Reachability Index construction recursive helper
 */
void IntermediateGraph::dfsUtil(IntermediateNode* nodeA, IntermediateNode* nodeB) {
    DFS_RI.insert(std::make_pair(nodeA, nodeB));
    for (IntermediateEdge* edge: nodeB->outgoingEdges) {
        auto result = DFS_RI.find(std::make_pair(nodeA, edge->endNode));
        if (result == DFS_RI.end()) {
            dfsUtil(nodeA, edge->endNode);
        }
    }
}

/**
 * mark the redundant edges using DFS
 */
void IntermediateGraph::markRedundantEdges_DFS() {
    constructDFSRI();

    //    For each vertex u in the graph:
    //    For each of its successors v:
    //    Check if there's a path from any other successor w of u to v
    //    If such a path exists, mark the edge (u, v) as redundant
    for (IntermediateNode* node: nodes) {
        for (IntermediateEdge* edge1: node->outgoingEdges) {
            for (IntermediateEdge* edge2: node->outgoingEdges) {
                if (edge1 != edge2
                    && !(edge2->isRedundant_DFS)
                    && DFS_RI.find(std::make_pair(edge2->endNode, edge1->endNode)) !=
                       DFS_RI.end()) {
                    edge1->isRedundant_DFS = true;
                    break;
                }
            }
        }
    }
}

//for transitive reduction based on TR-O+
void IntermediateGraph::constructBFLRI() {
    //traversed the graph and assign discoverTime and finishTime to each node
    for (IntermediateNode* node: startingNodes) {
        postOrderTraverse(node);
    }

    //make a shallow copy of nodes and sort it in post-order
    std::vector<IntermediateNode*> sortedNodes = nodes;
    auto comparator = [](const IntermediateNode* a, const IntermediateNode* b) {
        return a->finishTime < b->finishTime;
    };
    std::sort(sortedNodes.begin(), sortedNodes.end(), comparator);

    //embody the g map function
    uint64_t intervalLength = static_cast<uint64_t>(std::ceil(
            static_cast<double>(sortedNodes.size()) / static_cast<double>(numberOfIntervals)
    ));
    uint64_t i = 0;
    IntermediateNode* lowestNode;
    for (IntermediateNode* node: sortedNodes) {
        if (i % intervalLength == 0) {
            lowestNode = node;
        }
        gMap.emplace(node, lowestNode);
        i++;
    }

    //build BFL index for each node.
    for (IntermediateNode* node: sortedNodes) {
        if (node->LabelOut == nullptr) {
            computeLabelOut(node);
            computeLabelIn(node);
        }
    }
}

void IntermediateGraph::postOrderTraverse(IntermediateNode* node) {
    current++;
    node->discoverTime = current;
    for (IntermediateEdge* edge: node->outgoingEdges) {
        IntermediateNode* endNode = edge->endNode;
        if (!(endNode->isPostOrderAssigned)) {
            postOrderTraverse(endNode);
        }
    }
    node->isPostOrderAssigned = true;
    current++;
    node->finishTime = current;
}

uint64_t IntermediateGraph::hash(IntermediateNode* node) {
    uint64_t x = node->id;
    x = (x ^ (x >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    x = (x ^ (x >> 27)) * UINT64_C(0x94d049bb133111eb);
    x = x ^ (x >> 31);

    return (x % (numberOfHashValues)) + 1;
}

void IntermediateGraph::computeLabelOut(IntermediateNode* node) {
    node->LabelOut = std::make_unique<std::set<uint64_t>>
            (std::set < uint64_t > {hash(gMap.find(node)->second)});
    for (IntermediateEdge* edge: node->outgoingEdges) {
        if (edge->endNode->LabelOut == nullptr) {
            computeLabelOut(edge->endNode);
        }
        node->LabelOut->insert
                (edge->endNode->LabelOut->begin(), edge->endNode->LabelOut->end());
    }
}

void IntermediateGraph::computeLabelIn(IntermediateNode* node) {
    node->LabelIn = std::make_unique<std::set<uint64_t>>
            (std::set < uint64_t > {hash(gMap.find(node)->second)});
    for (IntermediateEdge* edge: node->incomingEdges) {
        if (edge->startNode->LabelIn == nullptr) {
            computeLabelIn(edge->startNode);
        }
        node->LabelIn->insert
                (edge->startNode->LabelIn->begin(), edge->startNode->LabelIn->end());
    }
}

bool IntermediateGraph::queryReachability_BFL(IntermediateNode* a, IntermediateNode* b) {
    visitedInQuery.clear();
    return isReachable_BFL(a, b);
}

bool IntermediateGraph::isReachable_BFL(IntermediateNode* a, IntermediateNode* b) {
    if (a == b)
        return true;
    visitedInQuery.insert(a);
    if ((a->discoverTime < b->discoverTime) && (a->finishTime > b->finishTime))
        return true;
    else if (!isSubset(b->LabelOut, a->LabelOut) || !isSubset(a->LabelIn, b->LabelIn))
        return false;
    else {
        for (IntermediateEdge* edge: a->outgoingEdges) {
            if (visitedInQuery.find(edge->endNode) == visitedInQuery.end() && isReachable_BFL(edge->endNode, b))
                return true;
        }
        return false;
    }
}

bool IntermediateGraph::isSubset(const std::unique_ptr<std::set<uint64_t>> &setA,
                                 const std::unique_ptr<std::set<uint64_t>> &setB) {
    if (!setA || !setB) return false;
    return std::includes(setB->begin(), setB->end(), setA->begin(), setA->end());
}

void IntermediateGraph::topoSort() {
    uint64_t i = 1;
    std::vector<IntermediateNode*> startingNodesCopy(startingNodes);
    while (!startingNodesCopy.empty()) {
        IntermediateNode* node = startingNodesCopy.front();
        startingNodesCopy.erase(startingNodesCopy.begin());
        node->topoOrder = i;
        i++;
        for (IntermediateEdge* edge: node->outgoingEdges) {
            IntermediateNode* m = (edge->endNode);
            edge->isTopoTraversed = true;
            bool mIsReady = true;
            for (IntermediateEdge* mEdge: m->incomingEdges) {
                if (!mEdge->isTopoTraversed)
                    mIsReady = false;
            }
            if (mIsReady) {
                startingNodesCopy.push_back(m);
            }
        }
    }

    for (IntermediateEdge* edge: edges)
        if (!edge->isTopoTraversed)
            throw std::runtime_error("This graph contains loop");

}

//bool IntermediateGraph::isRedundant_TROPlus(IntermediateEdge* edge) {
//    //select the faster way based on comparison between in-degree of end node and out-degree of starting node
//    //if the starting node can reach end node via another node, the edge is redundant
//    uint64_t queryCallsStartingNode = 0;
//    uint64_t queryCallsEndNode = 0;
//    std::vector<IntermediateEdge*> incomingEdgesToCheck;
//    std::vector<IntermediateEdge*> outgoingEdgesToCheck;
//    for(IntermediateEdge* incomingEdge: edge->endNode->incomingEdges){
//        if(!incomingEdge->isRedundant_TROPlus && incomingEdge->startNode->topoOrder > edge->startNode->topoOrder){
//            queryCallsEndNode++;
//            incomingEdgesToCheck.push_back(incomingEdge);
//        }
//    }
//    for(IntermediateEdge* outgoingEdge: edge->startNode->outgoingEdges){
//        if(!outgoingEdge->isRedundant_TROPlus && outgoingEdge->endNode->topoOrder < edge->endNode->topoOrder){
//            queryCallsEndNode++;
//            outgoingEdgesToCheck.push_back(outgoingEdge);
//        }
//    }
//
//    if (queryCallsStartingNode > queryCallsEndNode) {
//        for (IntermediateEdge* incomingEdge: incomingEdgesToCheck) {
//            return queryReachability_BFL(edge->startNode, incomingEdge->startNode);
//        }
//    } else {
//        for (IntermediateEdge* outgoingEdge: outgoingEdgesToCheck) {
//            return queryReachability_BFL(outgoingEdge->endNode, edge->endNode);
//        }
//    }
//    return false;
//}

bool IntermediateGraph::isRedundant_TROPlus(IntermediateEdge* edge) {
    if (edge->startNode->outgoingEdges.size() > edge->endNode->incomingEdges.size()) {
        for (IntermediateEdge* incomingEdge: edge->endNode->incomingEdges) {
            if (!incomingEdge->isRedundant_TROPlus
                && incomingEdge->startNode->topoOrder > edge->startNode->topoOrder
                && queryReachability_BFL(edge->startNode, incomingEdge->startNode))
                return true;
        }
    } else {
        for (IntermediateEdge* outgoingEdge: edge->startNode->outgoingEdges) {
            if (!outgoingEdge->isRedundant_TROPlus
                && outgoingEdge->endNode->topoOrder < edge->endNode->topoOrder
                && queryReachability_BFL(outgoingEdge->endNode, edge->endNode))
                return true;
        }
    }
    return false;
}

void IntermediateGraph::markRedundantEdges_TROPlus(bool withVerification) {
    constructBFLRI();
    topoSort();


    std::vector<IntermediateEdge*> sortedEdges;
    std::vector<IntermediateNodeWrapper*> nodeWrappers;

    //sort nodes based on in-degree or out-degree in ascending order
    for (IntermediateNode* node: nodes) {
        nodeWrappers.push_back(new IntermediateNodeWrapper(node, true));
        nodeWrappers.push_back(new IntermediateNodeWrapper(node, false));
    }

    std::sort(nodeWrappers.begin(), nodeWrappers.end(),
              [](IntermediateNodeWrapper* nodeWrapper1, IntermediateNodeWrapper* nodeWrapper2) {
                  return std::max(nodeWrapper1->inDegree, nodeWrapper1->outDegree) <
                         std::max(nodeWrapper2->inDegree, nodeWrapper2->outDegree);
              });

    // sort edges based on how fast it can be processed in redundancy check, which depends on
    // 1. in-degree of In-Node/out-degree of Out-Node
    // 2. topo-order of starting node of incoming Edges of In-Node/topo-order of end node of outgoing Edges of Out-Node
    for (IntermediateNodeWrapper* nodeWrapper: nodeWrappers) {
        if (nodeWrapper->isIn) {
            //sort the incoming edges of In-Node based on the descending topo-order of their starting nodes
            std::sort(nodeWrapper->node->incomingEdges.begin(), nodeWrapper->node->incomingEdges.end(),
                      [](IntermediateEdge* edge1, IntermediateEdge* edge2) {
                          return edge1->startNode->topoOrder > edge2->startNode->topoOrder;
                      });
            // add edges in the sorted edges vectors if they do not already exist and keep track of whether they are In-Node or Out-Node
            for (IntermediateEdge* edge: nodeWrapper->node->incomingEdges) {
                auto it = std::find(sortedEdges.begin(), sortedEdges.end(), edge);
                if (it == sortedEdges.end()) {
                    sortedEdges.push_back(edge);
                    if (withVerification) {
                        sortedEdgePairs.emplace_back(edge, true);
                    }
                }

            }
        } else {
            //sort the outgoing edges of Out-Node based on the ascending topo-order of their end nodes
            std::sort(nodeWrapper->node->outgoingEdges.begin(), nodeWrapper->node->outgoingEdges.end(),
                      [](IntermediateEdge* edge1, IntermediateEdge* edge2) {
                          return edge1->endNode->topoOrder < edge2->endNode->topoOrder;
                      });
            // add edges in the sorted edges vectors if they do not already exist and keep track of whether they are In-Node or Out-Node
            for (IntermediateEdge* edge: nodeWrapper->node->outgoingEdges) {
                auto it = std::find(sortedEdges.begin(), sortedEdges.end(), edge);
                if (it == sortedEdges.end()) {
                    sortedEdges.push_back(edge);
                    if (withVerification) {
                        sortedEdgePairs.emplace_back(edge, false);
                    }
                }
            }

        }
    }

    // check edges redundancy one edge at a time
    for (IntermediateEdge* edge: sortedEdges) {
        edge->isRedundant_TROPlus = isRedundant_TROPlus(edge);
    }
}














