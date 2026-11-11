#pragma once

#include <cstdint>
#include <iostream>

class Result {
    double totTime;
    double avg_run_time;
    double med_run_time;
    double standard_dev;
    double avg_frames_per_second;

  public:
    Result(uint32_t iterations, double _totTime, double* run_time_values);
    void Print(std::ostream& os, uint32_t index, std::string &backend, std::string &name);
    void WriteCSV(std::ostream& os, std::string& backend, std::string& name);
};
