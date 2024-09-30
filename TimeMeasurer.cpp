#include <vector>
#include "TimeMeasurer.h"
#include "GraphParser.h"
#include <iostream>
#include "IntermediateGraph.h"
#include <chrono>
#include <numeric>
#include <string>
#include <fstream>

void TimeMeasurer::measureGraphTRTime(std::string performanceFilePath, std::string graphFilePath) {
    std::vector<uint64_t> time_DFSRI;
    std::vector<uint64_t> time_DFS;
    std::vector<uint64_t> time_BFL;
    std::vector<uint64_t> time_TRO;

    for (int i = 0; i < 10; ++i) {
        FinalGraph* graph = GraphParser::importFinalGraph(graphFilePath);
        IntermediateGraph* intermediateGraph = GraphParser::parseToIntermediateGraph(graph);
        auto start1 = std::chrono::high_resolution_clock::now();
        intermediateGraph->constructDFSRI();
        auto start1part1 = std::chrono::high_resolution_clock::now();
        intermediateGraph->markRedundantEdges_DFS();
        auto start2 = std::chrono::high_resolution_clock::now();
        intermediateGraph->constructBFLRI();
        auto start2part1 = std::chrono::high_resolution_clock::now();
        intermediateGraph->markRedundantEdges_TROPlus(false);
        auto stop = std::chrono::high_resolution_clock::now();

        time_DFSRI.push_back(duration_cast<std::chrono::microseconds>(start1part1 - start1).count());
        time_DFS.push_back(duration_cast<std::chrono::microseconds>(start2 - start1).count());
        time_BFL.push_back(duration_cast<std::chrono::microseconds>(start2part1 - start2).count());
        time_TRO.push_back(duration_cast<std::chrono::microseconds>(stop - start2).count());
        delete intermediateGraph;
        delete graph;
        std::cout << "GOT " << i << std::endl;
    }
    double dfs_ri = calculateMean(time_DFSRI);
    double dfs = calculateMean(time_DFS);
    double bfl = calculateMean(time_BFL);
    double tro = calculateMean(time_TRO);

    std::ofstream file("algorithm_performance_data.csv", std::ios::app);
    file << graphFilePath << ","
         << dfs_ri << ","
         << dfs << ","
         << bfl << ","
         << tro << "\n";

    file.close();
}

double TimeMeasurer::startMeasurement(std::vector<std::string> filePaths) {
    std::ofstream file("algorithm_performance_data.csv");
    file << "graphName,DFS_RI,DFS,BFL,TRO\n";
    file.close();
    for (auto &filePath: filePaths) {
        TimeMeasurer::measureGraphTRTime("algorithm_performance_data.csv", filePath);
    }
}

double TimeMeasurer::calculateMean(const std::vector<uint64_t> &data) {
    if (data.empty()) {
        return 0.0;
    }
    uint64_t sum = std::accumulate(data.begin(), data.end(), uint64_t(0));
    return static_cast<double>(sum) / data.size();
}

