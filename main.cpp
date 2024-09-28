#include <iostream>
#include <cmath>
#include "IntermediateGraph.h"
#include <chrono>
#include <numeric>

#include "GraphParser.h"

int main() {
//        FinalGraph* graph = GraphParser::importFinalGraph("graphTestSparseN923E1422.txt");
//        FinalGraph* graph = GraphParser::importFinalGraph("graphTestDenseN996E13532.txt");
    std::vector<uint64_t> time_TROPlus;
    std::vector<uint64_t> time_DFS;
//      FinalGraph* graph = GraphParser::importFinalGraph("graphTestSparse.txt");
    for (int i = 0; i < 10; ++i) {
        FinalGraph* graph = GraphParser::importFinalGraph("graphTestSparse.txt");
        IntermediateGraph* intermediateGraph = GraphParser::parseToIntermediateGraph(graph);
        auto start1 = std::chrono::high_resolution_clock::now();
        intermediateGraph->markRedundantEdges_TROPlus();
        auto start2 = std::chrono::high_resolution_clock::now();
        intermediateGraph->markRedundantEdges_DFS();
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration_TROPlus = duration_cast<std::chrono::microseconds>(start2 - start1);
        auto duration_DFS = duration_cast<std::chrono::microseconds>(stop - start2);
        time_TROPlus.push_back(duration_TROPlus.count());
        time_DFS.push_back(duration_DFS.count());
    }
    double averageTRO = std::accumulate(time_TROPlus.begin(), time_TROPlus.end(), 0.0) /
                        static_cast<double>(time_TROPlus.size());
    double averageDFS =
            std::accumulate(time_DFS.begin(), time_DFS.end(), 0.0) / static_cast<double>(time_DFS.size());

    std::cout << "duration_TROPlus: " + std::to_string(averageTRO) << std::endl;
    std::cout << "duration_DFS: " + std::to_string(averageDFS) << std::endl;


//        FinalGraph* graph = GraphParser::importFinalGraph("graph.txt");
//        IntermediateGraph* intermediateGraph = GraphParser::parseToIntermediateGraph(graph);
//        intermediateGraph->markRedundantEdges_DFS();
//        std::cout << "DFS done " << std::endl;
//        intermediateGraph->markRedundantEdges_TROPlus();
//        std::cout << "TRO done" << std::endl;
//        for (const auto &item: intermediateGraph->edges) {
//            if (item->isRedundant_DFS != item->isRedundant_TROPlus) {
//                throw std::runtime_error("The result of two algorithms don't match");
//            }
//        }
//        return 0;
}
