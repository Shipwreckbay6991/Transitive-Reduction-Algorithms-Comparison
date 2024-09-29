
#ifndef ALGORITHMPROJECT_TIMEMEASURER_H
#define ALGORITHMPROJECT_TIMEMEASURER_H


class TimeMeasurer {
public:
    static void measureGraphTRTime(std::string performanceFilePath, std::string graphFilePath);

    static double calculateStandardDeviation(const std::vector<uint64_t> &data, double mean);

    static double startMeasurement(int startGraph, int endGraph);
};


#endif //ALGORITHMPROJECT_TIMEMEASURER_H
