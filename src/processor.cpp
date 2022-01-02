#include "processor.h"
#include "vector"
#include "linux_parser.h"


double Processor::Utilization(int cpu_id, int cpu_count) {
    size_t cpu_cnt = cpu_count;
    if(this->prevIdle.size() < cpu_cnt) {
        for (int i = 1; i <= cpu_count; i++)
        this->prevIdle.push_back(0);
    }
    if(this->prevTotal.size() < cpu_cnt) {
        for (int i = 1; i <= cpu_count; i++)
        this->prevTotal.push_back(0);
    }
    if(this->prevActive.size() < cpu_cnt) {
        for (int i = 1; i <= cpu_count; i++)
        this->prevActive.push_back(0);
    }

    if(this->prevIdle[cpu_id] == 0) {
        this->prevIdle[cpu_id] = LinuxParser::IdleJiffies(cpu_id);
        this->prevActive[cpu_id] = LinuxParser::ActiveJiffies(cpu_id);
        this->prevTotal[cpu_id] = this->prevIdle[cpu_id] + this->prevActive[cpu_id];
        return 0.0;
    }else {
        this->currentIdle = LinuxParser::IdleJiffies(cpu_id);
        this->currentActive = LinuxParser::ActiveJiffies(cpu_id);
        this->currentTotal = this->currentIdle + this->currentActive;
        double utilization = CalcuteLateUtilization(cpu_id);
        this->prevIdle[cpu_id] = this->currentIdle;
        this->prevActive[cpu_id] = this->currentActive;
        this->prevTotal[cpu_id] = this->currentTotal;
        return utilization;
    }
}

double Processor::CalcuteLateUtilization(int cpu_id) {
    int totald = this->currentTotal - this->prevTotal[cpu_id];
    int idled = this->currentIdle - this->prevIdle[cpu_id];

    return (totald-(idled))/(totald+0.000000000000001);
}