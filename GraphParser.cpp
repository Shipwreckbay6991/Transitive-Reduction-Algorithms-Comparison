
#include "GraphParser.h"
#include <fstream>
#include <iostream>


IntermediateGraph* GraphParser::parseToIntermediateGraph(FinalGraph* finalGraph) {
    IntermediateGraph* intermediateGraph = new IntermediateGraph();
    std::map<uint64_t, IntermediateNode*> nodeMap;
    for (FinalNode* finalNode: finalGraph->nodes) {
        IntermediateNode* node = new IntermediateNode(finalNode->id);
        nodeMap.emplace(finalNode->id, node);
        intermediateGraph->nodes.push_back(node);
    }
    for (FinalEdge* finalEdge: finalGraph->edges) {
        IntermediateEdge* edge = new IntermediateEdge(finalEdge->id);
        edge->startNode = nodeMap[finalEdge->startNodeId];
        edge->endNode = nodeMap[finalEdge->endNodeId];
        edge->startNode->outgoingEdges.push_back(edge);
        edge->endNode->incomingEdges.push_back(edge);
        intermediateGraph->edges.push_back(edge);
    }
    for (IntermediateNode* node: intermediateGraph->nodes) {
        if (node->incomingEdges.empty()) {
            intermediateGraph->startingNodes.push_back(node);
        }
    }
    return intermediateGraph;
}


FinalGraph* GraphParser::parseToFinalGraph(IntermediateGraph* intermediateGraph) {
    FinalGraph* finalGraph = new FinalGraph();
    for (const auto &item: intermediateGraph->nodes) {
        FinalNode* node = new FinalNode{item->id};
        finalGraph->nodes.push_back(node);
    }
    for (const auto &item: intermediateGraph->edges) {
        FinalEdge* edge = new FinalEdge{item->id, item->startNode->id, item->endNode->id};
        finalGraph->edges.push_back(edge);
    }
    return finalGraph;
}

/**
 * split the string into vector of strings based on given delimiter
 */
std::vector<std::string> split(std::string s, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

void GraphParser::exportFinalGraph(FinalGraph* finalGraph, std::string title) {
    std::ofstream file(title);
    file << finalGraph->nodes.size()
         << " "
         << finalGraph->edges.size()
         << std::endl;
    for (const auto &item: finalGraph->nodes) {
        file << item->id
             << std::endl;
    }
    for (const auto &item: finalGraph->edges) {
        file << item->id
             << " "
             << item->startNodeId
             << " "
             << item->endNodeId
             << std::endl;
    }
    file.close();
}

FinalGraph* GraphParser::importFinalGraph(std::string title) {
    FinalGraph* graph = new FinalGraph();
    std::ifstream file(title);
    std::string line;
    getline(file, line);
    auto header = split(line, " ");
    int numberOfNodes = std::atoi(header[0].c_str());
    int numberOfEdges = std::atoi(header[1].c_str());
    for (int i = 0; i < numberOfNodes; ++i) {
        FinalNode* node = new FinalNode();
        std::string nodeLine;
        getline(file, nodeLine);
        node->id = std::atoi(nodeLine.c_str());
        graph->nodes.push_back(node);
    }
    for (int i = 0; i < numberOfEdges; ++i) {
        FinalEdge* edge = new FinalEdge();
        std::string edgeLine;
        getline(file, edgeLine);
        auto edgeFields = split(edgeLine, " ");
        edge->id = std::atoi(edgeFields[0].c_str());
        edge->startNodeId = std::atoi(edgeFields[1].c_str());
        edge->endNodeId = std::atoi(edgeFields[2].c_str());
        graph->edges.push_back(edge);
    }
    file.close();
    return graph;
}

