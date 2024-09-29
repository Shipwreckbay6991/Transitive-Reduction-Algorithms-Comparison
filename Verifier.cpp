#include <string>
#include <iostream>
#include "Verifier.h"
#include "GraphParser.h"


bool Verifier::crossCheckTRCorrectness(std::string filePath) {
    FinalGraph* graph = GraphParser::importFinalGraph(filePath);
    IntermediateGraph* intermediateGraph = GraphParser::parseToIntermediateGraph(graph);
    intermediateGraph->markRedundantEdges_DFS();
    intermediateGraph->markRedundantEdges_TROPlus(false);
    for (const auto &item: intermediateGraph->edges) {
        if (item->isRedundant_DFS != item->isRedundant_TROPlus) {
            return false;
        }
    }
    return true;
}

bool Verifier::verifyGraphTopoOrder(std::string fileName) {
    FinalGraph* graph = GraphParser::importFinalGraph(fileName);
    IntermediateGraph* intermediateGraph = GraphParser::parseToIntermediateGraph(graph);
    for (IntermediateNode* node: intermediateGraph->startingNodes) {
        if (!verifyNodeTopoOrder(node))
            return false;
    }
    return true;
}

bool Verifier::verifyNodeTopoOrder(IntermediateNode* node) {
    for (IntermediateEdge* edge: node->outgoingEdges) {
        if (edge->endNode->topoOrder < node->topoOrder || !verifyNodeTopoOrder(edge->endNode))
            return false;
    }
    return true;
}

bool Verifier::verifyEdgesSortingOrder(std::string filePath) {
    FinalGraph* graph = GraphParser::importFinalGraph(filePath);
    IntermediateGraph* intermediateGraph = GraphParser::parseToIntermediateGraph(graph);

    intermediateGraph->markRedundantEdges_TROPlus(true);
    std::vector<std::pair<IntermediateEdge*, bool>> &sortedEdgePairs = intermediateGraph->sortedEdgePairs;
    for (int i = 0; i < static_cast<long long int>(intermediateGraph->sortedEdgePairs.size()) - 1; ++i) {

        //when both edges are added when examining end nodes
        if (sortedEdgePairs[i].second && sortedEdgePairs[i + 1].second) {
            //when both edges have the same end node
            if (sortedEdgePairs[i].first->endNode == sortedEdgePairs[i + 1].first->endNode) {
                //when both edges are not sorted by descending topo-order
                if (sortedEdgePairs[i].first->startNode->topoOrder <=
                    sortedEdgePairs[i + 1].first->startNode->topoOrder)
                    return false;
            }
                //when both edges have different end nodes
            else {
                //when both edges are not sorted by in-degree of end node
                if (sortedEdgePairs[i].first->endNode->incomingEdges.size() >
                    sortedEdgePairs[i + 1].first->endNode->incomingEdges.size()) {
                    return false;
                }
            }
        }
            //when both edges are added when examining measureGraphTRTime nodes
        else if (!sortedEdgePairs[i].second && !sortedEdgePairs[i + 1].second) {
            //when both edges have the same measureGraphTRTime node
            if (sortedEdgePairs[i].first->startNode == sortedEdgePairs[i + 1].first->startNode) {
                //when both edges are not sorted by ascending topo-order
                if (sortedEdgePairs[i].first->endNode->topoOrder >= sortedEdgePairs[i + 1].first->endNode->topoOrder)
                    return false;
            }
                //when both edges have different measureGraphTRTime nodes
            else {
                //when both edges are not sorted by out-degree of measureGraphTRTime node
                if (sortedEdgePairs[i].first->startNode->outgoingEdges.size() >
                    sortedEdgePairs[i + 1].first->startNode->outgoingEdges.size()) {
                    return false;
                }
            }
        } else {
            // when both edges are added when examining different types of nodes and are not sorted correctly
            if (sortedEdgePairs[i].second && !sortedEdgePairs[i + 1].second &&
                sortedEdgePairs[i].first->endNode->incomingEdges.size() >
                sortedEdgePairs[i + 1].first->startNode->outgoingEdges.size()) {
                return false;
            } else if (!sortedEdgePairs[i].second && sortedEdgePairs[i + 1].second &&
                       sortedEdgePairs[i].first->startNode->outgoingEdges.size() >
                       sortedEdgePairs[i + 1].first->endNode->incomingEdges.size()) {
                return false;
            }
        }
    }
    return true;
}
