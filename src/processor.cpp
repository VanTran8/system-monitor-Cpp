#include "processor.h"
#include "linux_parser.h"
#include <chrono>
#include <thread>

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
  auto prev_total = LinuxParser::Jiffies();
  auto prev_active = LinuxParser::ActiveJiffies();
  // sleep for 100ms
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  auto total = LinuxParser::Jiffies();
  auto active = LinuxParser::ActiveJiffies();
  auto total_diff = total - prev_total;
  auto active_diff = active - prev_active;
  return active_diff* 1.0 / total_diff;
}