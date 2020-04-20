#include <string>
#include <sstream>
#include <iomanip>
#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
    int ss = seconds % 60;
    int mm = (seconds / 60) % 60;
    int hh = seconds / 3600;
    std::ostringstream outstream;

    outstream << std::setw(2) << std::right << std::setfill('0') << hh << ":"
              << std::setw(2) << std::right << std::setfill('0') << mm << ":"
              << std::setw(2) << std::right << std::setfill('0') << ss;

    return outstream.str();
}
