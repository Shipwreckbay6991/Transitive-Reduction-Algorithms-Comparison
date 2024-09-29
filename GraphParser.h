
/**
 * @file GraphParser.h
 * @brief This file contains the GraphParser class which provides methods for parsing and converting between different graph representations
 */

#ifndef ALGORITHMPROJECT_GRAPHPARSER_H
#define ALGORITHMPROJECT_GRAPHPARSER_H


#include "IntermediateGraph.h"
#include "FinalGraph.h"

class GraphParser {
public:
    /**
     * @brief Converts a FinalGraph to an IntermediateGraph.
     *
     * This method creates an IntermediateGraph from a given FinalGraph. It maps nodes and edges from the FinalGraph
     * to their corresponding IntermediateGraph representations, maintaining the graph structure and connections.
     *
     * @param finalGraph Pointer to the input FinalGraph
     * @return Pointer to the newly created IntermediateGraph
     */
    static IntermediateGraph* parseToIntermediateGraph(FinalGraph* finalGraph);

    /**
     * @brief Converts an IntermediateGraph to a FinalGraph.
     *
     * This method creates a FinalGraph from a given IntermediateGraph. It maps nodes and edges from the IntermediateGraph
     * to their corresponding FinalGraph representations, preserving the graph structure.
     *
     * @param intermediateGraph Pointer to the input IntermediateGraph
     * @return Pointer to the newly created FinalGraph
     */
    static FinalGraph* parseToFinalGraph(IntermediateGraph* intermediateGraph);

    /**
     * @brief Exports a FinalGraph to a txt file.
     *
     * This method writes the structure of a FinalGraph to a file. The file format includes the number of nodes and edges,
     * followed by node IDs and edge details (ID, measureGraphTRTime node ID, end node ID).
     *
     * @param finalGraph Pointer to the FinalGraph to be exported
     * @param title The filename to write the graph data to
     */
    static void exportFinalGraph(FinalGraph* finalGraph, std::string title);

    /**
     * @brief Imports a FinalGraph from a txt file.
     *
     * This method reads a graph structure from a file and creates a corresponding FinalGraph object.
     * The file should be in the format produced by the exportFinalGraph method.
     *
     * @param title The filename to read the graph data from
     * @return Pointer to the newly created FinalGraph
     */
    static FinalGraph* importFinalGraph(std::string title);
};


#endif //ALGORITHMPROJECT_GRAPHPARSER_H
