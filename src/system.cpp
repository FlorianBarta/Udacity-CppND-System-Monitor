#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;



Processor& System::Cpu() { return cpus_;}


int System::CpuCount() { return LinuxParser::CpuCount();  }


vector<Process>& System::Processes() { 
    const vector<int> processes_pids = LinuxParser::Pids();

    processes_.clear();
    for (size_t i = 0; i < processes_pids.size(); ++i) {
        processes_.emplace_back(processes_pids.at(i));
    }

    std::sort(processes_.begin(), processes_.end());
    return processes_;
}


std::string System::Kernel() { return LinuxParser::Kernel(); }


float System::MemoryUtilization() { return LinuxParser::MemoryUtilization();}


std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }


int System::RunningProcesses() { return LinuxParser::RunningProcesses();  }


int System::TotalProcesses() { return LinuxParser::TotalProcesses();  }

long int System::UpTime() { return LinuxParser::UpTime(); }