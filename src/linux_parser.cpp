#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
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

// DONE: An example of how to read data from the filesystem
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string key, value, line;
  float total_mem, free_mem;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if(stream.is_open()) {
    while(std::getline(stream, line)) {
       std::istringstream linestream(line); 
       linestream >> key >> value;
       if(key == "MemTotal:") {
         total_mem = stof(value);
       }
       if(key == "MemFree:") {
         free_mem = stof(value);
       }
    }
  }
  return ((total_mem - free_mem)/total_mem);
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string up_time, idle_time, line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if(stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> up_time >> idle_time;
  }
  return stol(up_time); 
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  long active_jiffies;
  string line, value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if(stream.is_open()) {
    int i  = 0;
    std::getline(stream,line);
    std::istringstream linestream(line);
    while(linestream >> value) {
      if( i >= 13 && i <= 16) {
        active_jiffies += stol(value);
      }
      i++;
    }
  }
  return active_jiffies; 
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> cpus = LinuxParser::CpuUtilization();
  return std::stol(cpus[LinuxParser::CPUStates::kUser_]) + std::stol(cpus[LinuxParser::CPUStates::kNice_]) +                            std::stol(cpus[LinuxParser::CPUStates::kSystem_]) + std::stol(cpus[LinuxParser::CPUStates::kIRQ_]) +            std::stol(cpus[LinuxParser::CPUStates::kSoftIRQ_]) + std::stol(cpus[LinuxParser::CPUStates::kSteal_]);
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> cpus = LinuxParser::CpuUtilization();
  return std::stol(cpus[LinuxParser::CPUStates::kIdle_]) + std::stol(cpus[LinuxParser::CPUStates::kIOwait_]);
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  vector<string> cus;
  string parsed_str;
  string line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> parsed_str; // remove "cpu" string
    for(int i = LinuxParser::CPUStates::kUser_; i <= LinuxParser::CPUStates::kGuestNice_; i++) {
      linestream >> parsed_str;
      cus.push_back(parsed_str);
    }
  }
  return cus;
}

string LinuxParser::GetValueFromProcStat(string stat_key) 
{
  string key, value, line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) 
  {
    while (std::getline(stream, line)) 
    {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == stat_key) 
      {
        return value;
      }
    }
  }
  return string();
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string total_proc = LinuxParser::GetValueFromProcStat("processes");
  if(total_proc != "")
    return stoi(total_proc);
  return -1;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string running_proc = LinuxParser::GetValueFromProcStat("procs_running");
  if(running_proc != "")
    return stoi(running_proc);
  return -1; 
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string command;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if(stream.is_open()) {
    std::getline(stream, command);
    return command;
  }
  return string();
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string key, line, value;
  std::stringstream ram;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if(stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if(key == "VmSize:") {
        ram << std::fixed << std::setprecision(3) << stof(value) / 1000;
        return ram.str();
      }
    }
  }
  return string();
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string key, value, line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if(stream.is_open()) {
    while(std::getline(stream,line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if(key == "Uid:")
        return value;
    }
  }
  return string();
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string Uid = LinuxParser::Uid(pid);
  string key, line, value, temp;
  std::ifstream stream(kPasswordPath);
  if(stream.is_open()) {
    while (std::getline(stream,line)) {
      std::replace(line.begin(), line.end(), ':',' ');
      std::istringstream linestream(line);
      linestream >> key >> temp >> value;
      if(value == Uid)
        return key;
    }
  }
  return string();
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  long uptime;
  string line, value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if(stream.is_open()) {
    int i = 0;
    std::getline(stream,line);
    std::istringstream linestream(line);
    while(linestream >> value)
    {
      i++;
      if(i == 22) {
        uptime = stol(value) / sysconf(_SC_CLK_TCK);
        return uptime;
      }
    }
  }
  return -1;
}
