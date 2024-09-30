/**
 * @file TimeMeasurer.h
 * @brief This file contains the time measuring methods for transitive reduction  algorithms and their sub-algorithms
 */
#ifndef ALGORITHMPROJECT_TIMEMEASURER_H
#define ALGORITHMPROJECT_TIMEMEASURER_H


class TimeMeasurer {
public:
    /**
     * @brief measure time cost for different transitive algorithms and different reachability index construction approaches for a given graph file.
     */
    static void measureGraphTRTime(std::string performanceFilePath, std::string graphFilePath);

    /**
     * @brief measure time cost for different transitive algorithms and different reachability index construction approachee for given graph files
     */
    static double startMeasurement(std::vector<std::string> filePaths);

    /**
     * @brief calculate mean value of data in given vector
     */
    static double calculateMean(const std::vector<uint64_t> &data);
};


#endif //ALGORITHMPROJECT_TIMEMEASURER_H
