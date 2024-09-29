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
    std::vector<uint64_t> time_TROPlus;
    std::vector<uint64_t> time_DFS;
    for (int i = 0; i < 25; ++i) {
        FinalGraph* graph = GraphParser::importFinalGraph(graphFilePath);
        IntermediateGraph* intermediateGraph = GraphParser::parseToIntermediateGraph(graph);
        auto start1 = std::chrono::high_resolution_clock::now();
        intermediateGraph->markRedundantEdges_TROPlus(false);
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
    double varianceTRO = calculateStandardDeviation(time_TROPlus, averageTRO);
    double varianceDFS = calculateStandardDeviation(time_DFS, averageDFS);

    std::ofstream file("algorithm_performance_data.csv", std::ios::app);
    file << graphFilePath << ","
         << averageTRO << ","
         << averageDFS << ","
         << varianceTRO << ","
         << varianceDFS << "\n";

    file.close();
}

double TimeMeasurer::startMeasurement(int startGraph, int endGraph) {
    std::ofstream file("algorithm_performance_data.csv");
    file << "graphName,AVG_TRO,AVG_DFS,STD_TRO,STD_DFS\n";
    file.close();
    for (uint64_t i = startGraph; i <= endGraph; ++i) {
        TimeMeasurer::measureGraphTRTime("algorithm performance data",
                                         "/Users/linpingxiang/CLionProjects/AlgorithmProject/graphs/graph_" +
                                         std::to_string(i) + ".txt");
    }
}

double TimeMeasurer::calculateStandardDeviation(const std::vector<uint64_t> &data, double mean) {
    if (data.size() <= 1) {
        return 0.0;
    }

    auto variance_sum = std::transform_reduce(
            data.begin(), data.end(),
            0.0,
            std::plus<>(),
            [mean](double x) { return std::pow(x - mean, 2); }
    );

    return std::sqrt(variance_sum / static_cast<double>(data.size() - 1)); // Using n-1 for sample variance
}

