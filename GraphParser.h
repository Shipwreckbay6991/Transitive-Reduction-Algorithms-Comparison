//
// This graph parser is used to parse between FinalGraph and IntermediateGraph as well as
// import/export of FinalGraph to txt files
//

#ifndef ALGORITHMPROJECT_GRAPHPARSER_H
#define ALGORITHMPROJECT_GRAPHPARSER_H


#include "IntermediateGraph.h"
#include "FinalGraph.h"

class GraphParser {
public:
    static IntermediateGraph* parseToIntermediateGraph(FinalGraph* finalGraph);

    static FinalGraph* parseToFinalGraph(IntermediateGraph* intermediateGraph);

    static void exportFinalGraph(FinalGraph* finalGraph, std::string title);

    static FinalGraph* importFinalGraph(std::string title);
};


#endif //ALGORITHMPROJECT_GRAPHPARSER_H
