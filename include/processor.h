#ifndef PROCESSOR_H
#define PROCESSOR_H
#include <vector>


class Processor {
 public:
  float Utilization(int cpu_id, int cpu_count);  

  
 private:
  std::vector<int> prevTotal;
  std::vector<int> prevIdle;
  std::vector<int> prevActive;
  int currentTotal;
  int currentIdle;
  int currentActive;
  float CalcuteLateUtilization(int cpu_id);
};

#endif