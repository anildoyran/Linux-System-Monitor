#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() { return this->PID; }

float Process::CpuUtilization() const {
  long totalTime = LinuxParser::ActiveJiffies(this->PID) / sysconf(_SC_CLK_TCK);
  //long processStartTime = LinuxParser::ActiveJiffies(this->PID) / sysconf(_SC_CLK_TCK);
  long processRunTime = LinuxParser::UpTime(this->PID);
  /*return (LinuxParser::ActiveJiffies(this->PID) / sysconf(_SC_CLK_TCK)) /
         (LinuxParser::UpTime(this->PID) / 1000.0);*/
  return (processRunTime / (float) totalTime) / 1000;
}

string Process::Command() { return LinuxParser::Command(this->PID); }

string Process::Ram() { return LinuxParser::Ram(this->PID); }

string Process::User() { return LinuxParser::User(this->PID); }

long int Process::UpTime() { return LinuxParser::UpTime(this->PID); }

bool Process::operator<(Process const& a) const {
  return this->CpuUtilization() > a.CpuUtilization() ? false : true;
}

Process::Process(int _pid) : PID(_pid) {
  this->userName = User();
  this->command = Command();
  this->CPUUtilization = CpuUtilization();
  this->ram = Ram();
  this->upTime = UpTime();
}