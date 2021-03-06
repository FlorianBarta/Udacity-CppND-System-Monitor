#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(const int pid);
  int Pid();                               
  std::string User();                      
  std::string Command();                   
  float CpuUtilization(); 
  float calculateCpu(int pid);                  
  std::string Ram();                       
  long int UpTime();                       
  bool operator<(Process const& a) const;  

  
 private:
  int pid_ = 0;
  float cpu_utilization_ = 0.0;
};

#endif