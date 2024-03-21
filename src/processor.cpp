#include <thread>
#include <chrono>

#include "processor.h"
#include "linux_parser.h"


// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
  long preTotal = LinuxParser::Jiffies();
  long preIdle = LinuxParser::IdleJiffies();
  
  // sleep for 100ms
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  
  long total = LinuxParser::Jiffies();
  long idle = LinuxParser::IdleJiffies();
  
  long total_differ = total - preTotal;
  long idle_differ = idle - preIdle;
  return 1.0 * (total_differ - idle_differ) / total_differ; 
}