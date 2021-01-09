#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

Processor& System::Cpu() { return this->cpu_; }

bool System::CompareCPUUtil(Process& p1, Process& p2) {
  return (p1.CpuUtilization() > p2.CpuUtilization());
}

vector<Process>& System::Processes() {
  int PID;

  processes_.clear();

  for (unsigned int i = 0; i < LinuxParser::Pids().size(); i++) {
    PID = LinuxParser::Pids()[i];
    processes_.push_back(Process(PID));
  }

  std::sort(processes_.begin(), processes_.end(), System::CompareCPUUtil);
  return processes_;
}

std::string System::Kernel() { return LinuxParser::Kernel(); }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }