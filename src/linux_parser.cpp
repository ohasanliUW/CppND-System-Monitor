#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <numeric>
#include <iostream>

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
  string kernel, dummy, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> kernel >> dummy >> version;
  }
  return kernel + " " + version;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  //std::filesystem::path file(kProcDirectory);
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

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
    std::ifstream stream(kProcDirectory + kMeminfoFilename);
    long memTotal = 0, memFree = 0;

    if (stream.is_open()) {
        string line;

        while (std::getline(stream, line)) {
            std::istringstream linestream(line);
            string key;
            int val;

            linestream >> key >> val;

            // Remove ':' character from the key if any
            key.erase(std::remove(key.begin(), key.end(), ':'), key.end());
            if (key == "MemTotal") {
                memTotal = val;
            } else if (key == "MemFree") {
                memFree = val;
            }
        }
        return static_cast<float>(memTotal - memFree) / memTotal;
    }
    return 0.0;
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
    std::ifstream stream(kProcDirectory + kUptimeFilename);
    string line;
    long uptime;

    if (!stream.is_open()) {
        return -1;
    }

    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
    return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
    auto stats {LinuxParser::CpuUtilization()};

    long total = std::accumulate(stats.begin(),
                                 stats.end(),
                                 0,
                                 [](int t, std::string e) {return t + std::stol(e);});

    return total;
}

// Read process stats
static vector<string>
read_process_stats(int pid)
{
    std::ifstream statf(LinuxParser::kProcDirectory + to_string(pid) + LinuxParser::kStatFilename);
    vector<string> stats;

    if (!statf.is_open()) {
        return stats;
    }

    string line;
    if (!std::getline(statf, line)) {
        return stats;
    }

    // add all words into a vector
    std::istringstream iss(line);
    string word;
    while(iss >> word) {
        stats.push_back(word);
    }

    return stats;  
}

// TODO: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid)
{
    auto stats = read_process_stats(pid);

    if (stats.empty()) {
        return -1;
    }

    long int utime {std::stol(stats[13])};
    long int stime {std::stol(stats[14])};
    long int cutime {std::stol(stats[15])};
    long int cstime {std::stol(stats[16])};
    long int total = utime + stime + cutime + cstime;

    return total;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
    auto stats {LinuxParser::CpuUtilization()};

    return std::stol(stats[0]) + std::stol(stats[2]);
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
    auto stats = LinuxParser::CpuUtilization();
    return std::stol(stats[LinuxParser::kIdle_]) + std::stol(stats[LinuxParser::kIOwait_]);
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
    std::ifstream stream(kProcDirectory + kStatFilename);
    std::vector<string> words;

    if (!stream.is_open()) {
        return words;
    }

    string line, word;
    std::getline(stream, line);
    std::istringstream linestream(line);

    // ignore the first word
    linestream >> word;

    // push every other word into vector
    while(linestream >> word) {
        words.push_back(word);
    }
    return words;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
    string line;
    std::ifstream statsf(kProcDirectory + kStatFilename);

    if (!statsf.is_open()) {
        return -1;
    }

    while(std::getline(statsf, line)) {
        int numRunProcs = 0;
        string key;
        std::istringstream linestream(line);

        linestream >> key >> numRunProcs;
        if (key == "processes") {
            return numRunProcs;
        }
    }

    return -1;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
    string line;

    std::ifstream statsf(kProcDirectory + kStatFilename);
    if (!statsf.is_open()) {
        return -1;
    }

    while(std::getline(statsf, line)) {
        int numRunProcs = 0;
        string key;
        std::istringstream linestream(line);

        linestream >> key >> numRunProcs;
        if (key == "procs_running") {
            return numRunProcs;
        }
    }
    return -1;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
    string line;

    std::ifstream cmdf(kProcDirectory + to_string(pid) + kCmdlineFilename);
    if(!cmdf.is_open()) {
        return string();
    }

    std::getline(cmdf, line);
    return line;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
    string line;

    std::ifstream statusf(kProcDirectory + to_string(pid) + kStatusFilename);
    if (!statusf.is_open()) {
        return string();
    }

    while (std::getline(statusf, line)) {
        string key;
        int vmsize;
        std::istringstream linestream(line);

        linestream >> key;
        if (key == "VmSize:") {
            linestream >> vmsize;
            return std::to_string(vmsize / 1000);
        }
    }


    return string();
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
    string line;

    std::ifstream statusf(kProcDirectory + to_string(pid) + kStatusFilename);
    if (!statusf.is_open()) {
        return string();
    }

    while (std::getline(statusf, line)) {
        string key, uid;
        std::istringstream linestream(line);

        linestream >> key;
        if (key == "Uid:") {
            linestream >> uid;
            return uid;
        }
    }

    return string();
}

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid)
{
    string line, uid;

    uid = LinuxParser::Uid(pid);

    std::ifstream passwdf(kPasswordPath);
    if (!passwdf.is_open()) {
        return string();
    }

    // iterate lines of passwd file until user with uid is found
    while (std::getline(passwdf, line)) {
        string user, x, uid2;
        std::replace(line.begin(), line.end(), ':', ' ');
        std::istringstream iss(line);
        iss >> user >> x >> uid2;

        if (uid2 == uid) {
            return user;
        }
    } 

    return string();
}

// TODO: Read and return the uptime of a process
long LinuxParser::UpTime(int pid)
{
    auto stats = read_process_stats(pid);
    auto uptime = LinuxParser::UpTime();

    if (stats.empty() || uptime == -1) {
        return 0;
    }

    return uptime - std::stol(stats[21]) / sysconf(_SC_CLK_TCK);
}
