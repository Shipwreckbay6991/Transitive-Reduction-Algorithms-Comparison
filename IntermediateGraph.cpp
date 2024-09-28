//
// Created by Leah on 2024/8/14.
//

#include "IntermediateGraph.h"
#include <iostream>
#include <map>
#include <cmath>
#include <memory>
#include <algorithm>

//for graph creation
//IntermediateNode* IntermediateGraph::appendNode(IntermediateNode* baseNode) {
//    // Create new node and edge on the heap
//    IntermediateNode* newNode = new IntermediateNode(nextNodeId++);
//    IntermediateEdge* newEdge = new IntermediateEdge(nextEdgeId++);
//
//    // Set up the connections
//    newEdge->startNode = baseNode;
//    newEdge->endNode = newNode;
//
//    // Add the new edge to the lists
//    baseNode->outgoingEdges.push_back(newEdge);
//    newNode->incomingEdges.push_back(newEdge);
//    edges.push_back(newEdge);
//    nodes.push_back(newNode);
//    return newNode;
//}

//IntermediateNode* IntermediateGraph::createStartingNode() {
//    IntermediateNode* newNode = new IntermediateNode(nextNodeId++);
//    startingNodes.push_back(newNode);
//    nodes.push_back(newNode);
//    return newNode;
//}
//
//void IntermediateGraph::createEdge(IntermediateNode* startNode, IntermediateNode* endNode) {
//    IntermediateEdge* newEdge = new IntermediateEdge(nextEdgeId++);
//    newEdge->startNode = startNode;
//    newEdge->endNode = endNode;
//    startNode->outgoingEdges.push_back(newEdge);
//    endNode->incomingEdges.push_back(newEdge);
//    edges.push_back(newEdge);
//    startingNodes.erase(std::remove(startingNodes.begin(), startingNodes.end(), endNode), startingNodes.end());
//}


//For transitive reduction based on DFS
void IntermediateGraph::constructDFSRI() {
    for (IntermediateNode* node: nodes) {
        dfsUtil(node, node);
    }
}

void IntermediateGraph::dfsUtil(IntermediateNode* nodeA, IntermediateNode* nodeB) {
    DFS_RI.emplace_back(nodeA, nodeB);
    for (IntermediateEdge* edge: nodeB->outgoingEdges) {
        auto result = std::find(DFS_RI.begin(), DFS_RI.end(), std::make_pair(nodeA, edge->endNode));
        if (result == DFS_RI.end()) {
            dfsUtil(nodeA, edge->endNode);
        }
    }
}

void IntermediateGraph::markRedundantEdges_DFS() {
    //construct DFS RI
    constructDFSRI();

    //    For each vertex u in the graph:
    //    For each of its successors v:
    //    Check if there's a path from any other successor w of u to v
    //    If such a path exists, mark the edge (u, v) as redundant
    for (IntermediateNode* nodeA: nodes) {
        for (IntermediateEdge* edge1: nodeA->outgoingEdges) {
            for (IntermediateEdge* edge2: nodeA->outgoingEdges) {
                if (edge1 != edge2 && !(edge1->isRedundant_DFS) && !(edge2->isRedundant_DFS)) {
                    auto result = std::find(DFS_RI.begin(), DFS_RI.end(),
                                            std::make_pair(edge2->endNode, edge1->endNode));
                    if (result != DFS_RI.end()) {
                        edge1->isRedundant_DFS = true;
                    }
                }
            }
        }
    }
}

//for transitive reduction based on TR-O+
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

void IntermediateGraph::constructBFLRI() {
    //traversed the graph and assign discoverTime and finishTime to each node
    for (IntermediateNode* node: startingNodes) {
        assignPostOrder(node);
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

    //build BFL index for each node. Problematic.
    for (IntermediateNode* node: sortedNodes) {
        if (node->LabelOut == nullptr) {
            computeLabelOut(node);
            computeLabelIn(node);
        }
    }
}

void IntermediateGraph::assignPostOrder(IntermediateNode* node) {
    current++;
    node->discoverTime = current;
    for (IntermediateEdge* edge: node->outgoingEdges) {
        IntermediateNode* endNode = edge->endNode;
        if (!(endNode->isPostOrderAssigned)) {
            assignPostOrder(endNode);
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
    visitedInQuery.push_back(a);
    if ((a->discoverTime < b->discoverTime) && (a->finishTime > b->finishTime))
        return true;
    else if (!isSubset(b->LabelOut, a->LabelOut) || !isSubset(a->LabelIn, b->LabelIn))
        return false;
    else {
        for (IntermediateEdge* edge: a->outgoingEdges) {
            auto it = std::find(visitedInQuery.begin(), visitedInQuery.end(), edge->endNode);
            if (it == visitedInQuery.end() && isReachable_BFL(edge->endNode, b))
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

bool IntermediateGraph::isRedundant_TROPlus(IntermediateEdge* edge) {
    if (edge->startNode->outgoingEdges.size() > edge->endNode->incomingEdges.size()) {
        for (IntermediateEdge* incomingEdge: edge->endNode->incomingEdges) {
            if (incomingEdge->startNode->topoOrder > edge->startNode->topoOrder &&
                queryReachability_BFL(edge->startNode, incomingEdge->startNode))
                return true;
        }
    } else {
        for (IntermediateEdge* outgoingEdge: edge->startNode->outgoingEdges) {
            if (outgoingEdge->endNode->topoOrder < edge->endNode->topoOrder &&
                queryReachability_BFL(outgoingEdge->endNode, edge->endNode))
                return true;
        }
    }
    return false;
}

void IntermediateGraph::markRedundantEdges_TROPlus() {
    constructBFLRI();
    topoSort();
    std::vector<IntermediateEdge*> sortedEdges;
    std::vector<std::pair<IntermediateEdge*, bool>> sortedEdgePairs;
    std::vector<IntermediateNodeWrapper*> nodeWrappers;

    //sort nodes based on in-degree or out-degree
    for (IntermediateNode* node: nodes) {
        nodeWrappers.push_back(new IntermediateNodeWrapper(node, true));
        nodeWrappers.push_back(new IntermediateNodeWrapper(node, false));
    }
    std::sort(nodeWrappers.begin(), nodeWrappers.end(),
              [](IntermediateNodeWrapper* nodeWrapper1, IntermediateNodeWrapper* nodeWrapper2) {
                  return std::max(nodeWrapper1->inDegree, nodeWrapper1->outDegree) <
                         std::max(nodeWrapper2->inDegree, nodeWrapper2->outDegree);
              });

    //sort edges based on how fast it can be processed in redundancy check
    for (IntermediateNodeWrapper* nodeWrapper: nodeWrappers) {
        if (nodeWrapper->isIn) {
            std::sort(nodeWrapper->node->incomingEdges.begin(), nodeWrapper->node->incomingEdges.end(),
                      [](IntermediateEdge* edge1, IntermediateEdge* edge2) {
                          return edge1->startNode->topoOrder > edge2->startNode->topoOrder;
                      });
            for (IntermediateEdge* edge: nodeWrapper->node->incomingEdges) {
                auto it = std::find(sortedEdges.begin(), sortedEdges.end(), edge);
                if (it == sortedEdges.end()) {
                    sortedEdges.push_back(edge);
                    sortedEdgePairs.push_back(std::make_pair(edge, true));
                }

            }
        } else {
            std::sort(nodeWrapper->node->outgoingEdges.begin(), nodeWrapper->node->outgoingEdges.end(),
                      [](IntermediateEdge* edge1, IntermediateEdge* edge2) {
                          return edge1->endNode->topoOrder < edge2->endNode->topoOrder;
                      });

            for (IntermediateEdge* edge: nodeWrapper->node->outgoingEdges) {
                auto it = std::find(sortedEdges.begin(), sortedEdges.end(), edge);
                if (it == sortedEdges.end()) {
                    sortedEdges.push_back(edge);
                    sortedEdgePairs.push_back(std::make_pair(edge, false));
                }
            }

        }
    }

//    //verify the order of sorted edges
//    for (int i = 0; i < sortedEdgePairs.size() - 1; ++i) {
//        //when both edges are added when examining end nodes
//        if (sortedEdgePairs[i].second && sortedEdgePairs[i + 1].second) {
//            //when both edges have the same end node
//            if (sortedEdgePairs[i].first->endNode == sortedEdgePairs[i + 1].first->endNode) {
//                //when both edges are not sorted by descending topo-order
//                if (sortedEdgePairs[i].first->startNode->topoOrder <=
//                    sortedEdgePairs[i + 1].first->startNode->topoOrder)
//                    throw std::runtime_error(
//                            "For same node, the incoming edges are not sorted in descending topo-order of its start node");
//            }
//                //when both edges have different end nodes
//            else {
//                //when both edges are not sorted by in-degree of end node
//                if (sortedEdgePairs[i].first->endNode->incomingEdges.size() >
//                    sortedEdgePairs[i + 1].first->endNode->incomingEdges.size()) {
//                    throw std::runtime_error("The nodes are not sorted based on in-degree correctly");
//                }
//            }
//        }
//            //when both edges are added when examining start nodes
//        else if (!sortedEdgePairs[i].second && !sortedEdgePairs[i + 1].second) {
//            //when both edges have the same start node
//            if (sortedEdgePairs[i].first->startNode == sortedEdgePairs[i + 1].first->startNode) {
//                //when both edges are not sorted by ascending topo-order
//                if (sortedEdgePairs[i].first->endNode->topoOrder >= sortedEdgePairs[i + 1].first->endNode->topoOrder)
//                    throw std::runtime_error(
//                            "For same node, the outgoing edges are not sorted in ascending topo-order of its end node");
//            }
//                //when both edges have different start nodes
//            else {
//                //when both edges are not sorted by out-degree of start node
//                if (sortedEdgePairs[i].first->startNode->outgoingEdges.size() >
//                    sortedEdgePairs[i + 1].first->startNode->outgoingEdges.size()) {
//                    throw std::runtime_error("The nodes are not sorted based on out-degree correctly");
//                }
//            }
//        } else {
//            // when both edges are added when examining different types of nodes and are not sorted correctly
//            if (sortedEdgePairs[i].second && !sortedEdgePairs[i + 1].second &&
//                sortedEdgePairs[i].first->endNode->incomingEdges.size() >
//                sortedEdgePairs[i + 1].first->startNode->outgoingEdges.size()) {
//                throw std::runtime_error("The nodes are not sorted based on in/out-degree correctly");
//            } else if (!sortedEdgePairs[i].second && sortedEdgePairs[i + 1].second &&
//                       sortedEdgePairs[i].first->startNode->outgoingEdges.size() >
//                       sortedEdgePairs[i + 1].first->endNode->incomingEdges.size()) {
//                throw std::runtime_error("The nodes are not sorted based on in/out-degree correctly");
//            }
//        }
//    }

    // check edges redundancy one edge at a time
    while (!sortedEdges.empty()) {
        IntermediateEdge* edgeUnderCheck = sortedEdges.front();
        sortedEdges.erase(sortedEdges.begin());
        edgeUnderCheck->isRedundant_TROPlus = isRedundant_TROPlus(edgeUnderCheck);
    }
}

//void IntermediateGraph::removeRedundantEdges_DFS() {
//    for (IntermediateNode* node: nodes) {
//        erase_if(node->outgoingEdges, [](IntermediateEdge* edge) { return edge->isRedundant_DFS; });
//        erase_if(node->incomingEdges, [](IntermediateEdge* edge) { return edge->isRedundant_DFS; });
//    }
//    std::erase_if(edges, [](IntermediateEdge* edge) {
//        if (edge->isRedundant_DFS) {
//            delete edge;
//            return true;
//        }
//        return false;
//    });
//}
//
//void IntermediateGraph::removeRedundantEdges_TROPlus() {
//    for (IntermediateNode* node: nodes) {
//        erase_if(node->outgoingEdges, [](IntermediateEdge* edge) { return edge->isRedundant_TROPlus; });
//        erase_if(node->incomingEdges, [](IntermediateEdge* edge) { return edge->isRedundant_TROPlus; });
//    }
//    std::erase_if(edges, [](IntermediateEdge* edge) {
//        if (edge->isRedundant_TROPlus) {
//            delete edge;
//            return true;
//        }
//        return false;
//    });
//}














