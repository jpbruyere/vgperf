#include <Result.hpp>

#include <iomanip>
#include <cmath>

double median_run_time(double data[], int n) {
    double temp;
    int    i, j;
    for (i = 0; i < n; i++)
        for (j = i + 1; j < n; j++) {
            if (data[i] > data[j]) {
                temp    = data[j];
                data[j] = data[i];
                data[i] = temp;
            }
        }
    if (n % 2 == 0)
        return ((data[n / 2] + data[n / 2 - 1]) / 2);
    return data[n / 2];
}
double standard_deviation(const double data[], int n, double mean) {
    double sum_deviation = 0.0;
    int    i;
    for (i = 0; i < n; ++i)
        sum_deviation += (data[i] - mean) * (data[i] - mean);
    return sqrt(sum_deviation / n);
}

Result::Result(uint32_t iterations, double _totTime, double* run_time_values) {
    if (run_time_values) {
        double count    = iterations;
        this->totTime   = _totTime;
        avg_run_time    = totTime / count;
        med_run_time    = median_run_time(run_time_values, count);
        standard_dev    = standard_deviation(run_time_values, count, avg_run_time);
        avg_frames_per_second = (1.0 / avg_run_time);
    }
}

void Result::Print(std::ostream& os, uint32_t index, std::string& backend, std::string& name) {
    os.imbue(std::locale("en_US.utf8"));
    os << "\uFF5C "<< std::right << std::setw(3) << std::setprecision(3) << index << " \uFF5C ";
    os << std::left << std::setw(10) << backend << "\uFF5C";
    os << std::left << std::setw(36) << name << "\uFF5C";
    os << std::right << std::setw(10) << std::setprecision(5) << std::fixed << this->avg_run_time << " \uFF5C ";
    os << std::right << std::setw(10) << std::setprecision(5) << std::fixed << this->med_run_time << " \uFF5C ";
    os << std::right << std::setw(10) << std::setprecision(5) << std::fixed << this->standard_dev << " \uFF5C ";
    //std::cout << std::right << std::setw(10) << std::setprecision(3) << std::fixed << this->totTime * 1000 / this->app->iterations << " \uFF5C ";
    os << std::endl;
    /*for (int var = 0; var < count; ++var) {
        std::cout << std::right << std::setw(10) << std::setprecision(5) << std::fixed << run_time_values[var]<< ";";
    }
    std::cout << std::endl;*/
}
void Result::WriteCSV(std::ostream& os, std::string& backend, std::string& name) {
    os.imbue(std::locale("en_US.utf8"));
    os << backend << ";";
    os << name << ";";
    os << std::setprecision(5) << std::fixed << this->avg_run_time << ";";
    os << std::setprecision(5) << std::fixed << this->med_run_time << ";";
    os << std::setprecision(5) << std::fixed << this->standard_dev << ";";
    os << std::endl;
}

