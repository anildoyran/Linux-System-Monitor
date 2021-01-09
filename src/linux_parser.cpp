#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>

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
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
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
  string line, key;
  int value, freeMemory, totalMemory;

  std::ifstream file(kProcDirectory + kMeminfoFilename);

  if (file.is_open()) {
    while (std::getline(file, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal") {
          totalMemory = value;
        } else if (key == "MemFree") {
          freeMemory = value;
        }
      }
    }
  }
  return (float)(totalMemory - freeMemory) / (float)totalMemory;
}

long LinuxParser::UpTime() {
  long sysUpTime;
  string line;
  std::ifstream file(kProcDirectory + kUptimeFilename);
  if (file.is_open()) {
    std::getline(file, line);
    std::replace(line.begin(), line.end(), '.', ' ');
    std::istringstream linestream(line);
    linestream >> sysUpTime;
  }
  return sysUpTime;
}

long LinuxParser::Jiffies() {
  long ret = 0;

  for (unsigned int i = 0; i < CpuUtilization().size(); i++) {
    ret += stol(CpuUtilization()[i]);
  }
  return ret;
}

long LinuxParser::ActiveJiffies(int pid) {
  string PID = std::to_string(pid);
  long procStartTime = 0;
  string line, data;
  int cnt = 1;

  std::ifstream file(kProcDirectory + PID + kStatFilename);
  if (file.is_open()) {
    std::getline(file, line);
    std::stringstream linestream(line);
    while (linestream >> data) {
      if ((cnt > 12 && cnt < 18) || cnt == 22) {
        procStartTime += stol(data);
      }      
      cnt++;
    }
  }
  return procStartTime;
}

long LinuxParser::ActiveJiffies() {
  long ret = 0;;

  for (unsigned int i = 0; i < CpuUtilization().size(); i++) {
    if (i != IDLE_SLOT && i != IOWAIT_SLOT) {
      ret += stol(CpuUtilization()[i]);
    }
  }
  return ret;
}

long LinuxParser::IdleJiffies() {
  return std::stol(CpuUtilization()[IDLE_SLOT]) +
         std::stol(CpuUtilization()[IOWAIT_SLOT]);
}

vector<string> LinuxParser::CpuUtilization() {
  std::vector<string> CPUData;
  string line, data;

  std::ifstream file(kProcDirectory + kStatFilename);
  if (file.is_open()) {
    std::getline(file, line);
    std::stringstream linestream(line);
    while (linestream >> data) {
      if ("cpu" != data) {
        CPUData.push_back(data);
      }
    }
  }
  return CPUData;
}

int LinuxParser::TotalProcesses() {
  string line, header;
  int data;

  std::ifstream file(kProcDirectory + kStatFilename);
  if (file.is_open()) {
    while (std::getline(file, line)) {
      std::istringstream linestream(line);
      while (linestream >> header >> data) {
        if ("processes" == header) {
          return data;
        }
      }
    }
  }
  return EXIT_FAILURE;
}

int LinuxParser::RunningProcesses() {
  string line, header;
  int data;

  std::ifstream file(kProcDirectory + kStatFilename);
  if (file.is_open()) {
    while (std::getline(file, line)) {
      std::istringstream linestream(line);
      while (linestream >> header >> data) {
        if ("procs_running" == header) {
          return data;
        }
      }
    }
  }
  return EXIT_FAILURE;
}

string LinuxParser::Command(int pid) {
  string PID = std::to_string(pid);
  string line, data, cmd;

  std::ifstream file(kProcDirectory + PID + kCmdlineFilename);
  if (file.is_open()) {
    while (std::getline(file, line)) {
      std::istringstream linestream(line);
      while (linestream >> data) {
        cmd = cmd + data + " ";
      }
    }
  }
  return cmd;
}

string LinuxParser::Ram(int pid) {
  string PID = std::to_string(pid);
  string line, header, data, memUtil;

  std::ifstream file(kProcDirectory + PID + kStatusFilename);
  if (file.is_open()) {
    while (std::getline(file, line)) {
      std::istringstream linestream(line);
      while (linestream >> header >> data) {
        if ("VmSize:" == header) {
          memUtil = data;
        }
      }
    }
  }
  return std::to_string(std::stoi(memUtil) / 1024);
}

string LinuxParser::Uid(int pid) {
  string PID = std::to_string(pid);
  string line, header, data, UID;

  std::ifstream file(kProcDirectory + PID + kStatusFilename);
  if (file.is_open()) {
    while (std::getline(file, line)) {
      std::istringstream linestream(line);
      while (linestream >> header >> data) {
        if ("Uid:" == header) {
          UID = data;
        }
      }
    }
  }
  return UID;
}

string LinuxParser::User(int pid) {
  string UID = LinuxParser::Uid(pid);
  string line, header, data, domain;

  std::ifstream file(kPasswordPath);
  if (file.is_open()) {
    while (std::getline(file, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> header >> domain >> data) {
        if (UID == data) {
          return header;
        }
      }
    }
  }
  return "fail";
}

long LinuxParser::UpTime(int pid) {
  string PID = std::to_string(pid);
  long procStartTime;
  string line, data;
  int cnt = 1;

  std::ifstream file(kProcDirectory + PID + kStatFilename);
  if (file.is_open()) {
    std::getline(file, line);
    std::stringstream linestream(line);
    while (linestream >> data) {
      if (cnt == 22) {
        procStartTime = stol(data);
      }
      cnt++;
    }
  }
  return LinuxParser::UpTime() - procStartTime / sysconf(_SC_CLK_TCK);
}