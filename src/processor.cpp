#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  float upTime, idleJiffies, totalJiffies;
  float TimeDelta, IdleDelta, TotalDelta;

  upTime = LinuxParser::UpTime();
  totalJiffies = LinuxParser::Jiffies();
  idleJiffies = LinuxParser::IdleJiffies();

  TimeDelta = upTime - time_;
  IdleDelta = idleJiffies - idle_;
  TotalDelta = totalJiffies - total_;

  time_ = upTime;
  idle_ = idleJiffies;
  total_ = totalJiffies;

  return ((TotalDelta - IdleDelta) / TotalDelta) / TimeDelta;
}