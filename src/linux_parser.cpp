#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <cmath>

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
  string os, version, kernel;
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
  string key, line;
  float mem_total, mem_free;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> key >> mem_total;
    std::getline(filestream, line);
    std::istringstream linestream2(line);
    linestream2 >> key >> mem_free;
  }
  return (mem_total - mem_free)/mem_total; 
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  float uptime;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream) {
    string line;
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime; 
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies(); }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  string line, value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  long result = 0;
  if (filestream) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 17; i++) {
      linestream >> value;
      if (i == 13 || i == 14 || i == 15 || i == 16) {
        result += std::stol(value);
      }
    }
  }
  return result; 
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  vector<long> cpus = LinuxParser::CpuUtilization();
  return cpus[LinuxParser::CPUStates::kUser_] + cpus[LinuxParser::CPUStates::kNice_] + cpus[LinuxParser::CPUStates::kSystem_] + cpus[LinuxParser::CPUStates::kIRQ_] + cpus[LinuxParser::CPUStates::kSoftIRQ_] + cpus[LinuxParser::CPUStates::kSteal_]; 
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  vector<long> cpus = LinuxParser::CpuUtilization();
  return cpus[LinuxParser::CPUStates::kIdle_] + cpus[LinuxParser::CPUStates::kIOwait_]; }

// TODO: Read and return CPU utilization
vector<long> LinuxParser::CpuUtilization() { 
  string line, cpu;
  long value;
  vector<long> cpus;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    for (int i = LinuxParser::CPUStates::kUser_; i < LinuxParser::CPUStates::kGuestNice_; i++) {
      linestream >> value;
      cpus.push_back(value);      
    }  
  }
  return cpus; } // changed string to int

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string key, line;
  int total_procs;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "processes") {
        linestream >> total_procs;
        break;
      }
    }
  }
  return total_procs; 
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string key, line;
  int running_procs;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "procs_running") {
        linestream >> running_procs;
        break;
      }
    }
  }
  return running_procs; 
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (filestream) {
    std::getline(filestream, line);
  }
  return line; 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::Ram(int pid) { // changed string to long
  string line, key;
  long ram;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "VmSize:") {
        linestream >> ram;
        break;
      }
    }
  }
    ram = round(1.0 * ram / 1024); // convert to MG
  return ram; 
} 

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string line, key, uid;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "Uid:") {
        linestream >> uid;
        break;
      }
    }
  } 
  return uid; 
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string user, x, uid, line;
  std::ifstream filestream(kPasswordPath);
  if (filestream) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> x >> uid;
      if (uid == LinuxParser::Uid(pid)) 
        break;
    }
  }
  return user;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  string line, value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 22; i++) {
      linestream >> value;
    }
  }
  return std::stol(value) / sysconf(_SC_CLK_TCK); 
}