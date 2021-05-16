#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;
#include "linux_parser.h"

Process::Process(const int pid) : pid_(pid) {
   calculateCpu(pid);
}

float Process::calculateCpu(int pid) {
    long activeJiffies = LinuxParser::ActiveJiffiesPid(pid);
    long seconds = LinuxParser::UpTime() + 1 - LinuxParser::UpTime(pid);
    float value = 100 * ((activeJiffies * 1.0 / sysconf(_SC_CLK_TCK)) / (seconds * 1.0));
    this->cpu_utilization_ = value;
    return value;
}

int Process::Pid() { return this->pid_; }


float Process::CpuUtilization() {return calculateCpu(this->pid_);}

string Process::Command() { return LinuxParser::Command(this->pid_);}

string Process::Ram() { return to_string((int)LinuxParser::Ram(this->pid_));}

string Process::User() { return LinuxParser::User(this->pid_);}

long int Process::UpTime() { return LinuxParser::UpTime(this->pid_); }


bool Process::operator<(Process const& a) const {
  return (this->cpu_utilization_ > a.cpu_utilization_)? true : false;
}