#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}


float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  string value;
  float totalMem;
  float freeMem;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          totalMem = std::stof(value);
        } else if (key == "MemAvailable:") {
          freeMem = std::stof(value);
        }
      }
    }
  }
  filestream.close();
  float utilization = (totalMem-freeMem)/totalMem;
  return utilization;
}


long LinuxParser::UpTime() {
  long uptime = 0;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
    return uptime;
  }
  return uptime;
}

long LinuxParser::Jiffies() {
  string line;
  string cpuString;
  long value;
  long total_jiffies = 0;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> cpuString;
      if(cpuString == "cpu") {
        while (linestream >> value) {
          total_jiffies += value;
        }
        break;
      }
    }
  }
  return total_jiffies;
}

long LinuxParser::Jiffies(int cpuId) {
  string line;
  string cpuString;
  long value;
  long total_jiffies = 0;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> cpuString;
      if(cpuString == "cpu"+std::to_string(cpuId-1)) {
        while (linestream >> value) {
          total_jiffies += value;
        }
        break;
      }
    }
  }
  return total_jiffies;
}


long LinuxParser::ActiveJiffiesPid(int pid) {
  string line;
  string value = "";
  long active_jiffies = 0;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    int i = 1;
    while (linestream >> value) { 
      if(i == 14 || i == 15 || i == 16 || i == 17) {
        return stof(value);
        active_jiffies += stof(value);
      }
      i++;
    }
  }
  return active_jiffies;
}


long LinuxParser::ActiveJiffies(int cpuId) {
  string line;
  string cpuString;
  long value;
  long active_jiffies = 0;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> cpuString;
      if(cpuString == "cpu"+std::to_string(cpuId-1)) {
        int i = 0;
        while (linestream >> value) {
          i++;
          if(i == CPUStates::kUser_ || i == CPUStates::kNice_ || i == CPUStates::kSystem_ 
          || i == CPUStates::kIRQ_ || i == CPUStates::kSoftIRQ_ || i == CPUStates::kSteal_) {
            active_jiffies += value;
          }
        }
        break;
      }
    }
  }
  return active_jiffies;
}

long LinuxParser::IdleJiffies(int cpuId) {
  string line;
  string cpuString;
  long value;
  long idleJiffies = 0;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> cpuString;
      if(cpuString == "cpu"+std::to_string(cpuId-1)) {
        int i = 0;
        while (linestream >> value) {
          i++;
          if(i == CPUStates::kIdle_ || i == CPUStates::kIOwait_) {
            idleJiffies += value;
          }
        }
        break;
      }
    }
  }
  return idleJiffies;
}

int LinuxParser::CpuCount() {  
  string line;
  string key;
  string value;
  int count = 0;
  std::ifstream filestream(kProcDirectory + kCpuinfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processor") {
          count++;
        }
      }
    }
  }
  return count;
}


int LinuxParser::TotalProcesses() { 
  string line;
  string key;
  string value;
  int total;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          total = std::stoi(value);
          break;
        } 
      }
    }
  }
  return total;
}


int LinuxParser::RunningProcesses() {
  string line;
  string key;
  string value;
  int running;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          running = std::stoi(value);
          break;
        } 
      }
    }
  }
  return running;
}


string LinuxParser::Command(int pid) {
  string line = "";
  std::ifstream filestream(kProcDirectory + to_string(pid)+ kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
  }
  return line;
}


float LinuxParser::Ram(int pid) {
  string line;
  string key;
  float value = 0.0;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        //Used VmData instead of VmSize because Physical Memory is more significantly
        if (key == "VmData:") {
          return value / 1000;
          break;
        } 
      }
    }
  }
  
  return value;
}

string LinuxParser::User(int pid) { 
  string line;
  string key;
  string value;
  string name;
  string x;
  string uid;
  string output;
  int u_id;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          u_id = std::stoi(value);
          break;
        } 
      }
    }
  }
  std::ifstream filestream_pw(kPasswordPath);
  if (filestream_pw.is_open()) {
    while (std::getline(filestream_pw, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream_pw(line);
      while (linestream_pw >> name >> x >> uid) {
        if (uid == to_string(u_id)) {
          return name;
          break;
        } 
      }
    }
  }
  return output;
}


long LinuxParser::UpTime(int pid) {
  string line;
  string key;
  long value = 0;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 21; ++i) {
      linestream >> key;
    }
    linestream >> value;
    int upTimePid = UpTime() - value/sysconf(_SC_CLK_TCK);
    return upTimePid;
  }
  return value;
}
