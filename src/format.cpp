#include <string>
#include <sstream>

#include "format.h"

using std::string;

string Format::formatInt(const int num) {
    if(num < 10) { 
        return "0"+ std::to_string(num);
    } else {
        return std::to_string(num);
    }
}

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
    int h, m, s;

    h = seconds / 3600;
    seconds = seconds%3600;
    m = seconds / 60;
    seconds = seconds%60;
    s = seconds;

    std::ostringstream outStr;
    outStr << formatInt(h) << ":" << formatInt(m) << ":" << formatInt(s);
    return outStr.str();
}

