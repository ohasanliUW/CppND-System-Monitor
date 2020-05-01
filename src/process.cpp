#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// C-Tor
Process::Process(int pid) : pid(pid) {
    user = LinuxParser::User(pid);
    command = LinuxParser::Command(pid);
    cpu = 0.0f;
}

// Return this process's ID
int Process::Pid() { return pid; }

// Return this process's CPU utilization
float Process::CpuUtilization() {
    auto total_time = LinuxParser::ActiveJiffies(pid);
    auto seconds = LinuxParser::UpTime(pid);

    cpu = (static_cast<float>(total_time) / sysconf(_SC_CLK_TCK)) / seconds;
    return cpu;
}

// TODO: Return the command that generated this process
string Process::Command() {
    command = LinuxParser::Command(pid);
    return command;
}

// Return this process's memory utilization
string Process::Ram() {
    ram = LinuxParser::Ram(pid);
    return ram;
}

// Return the user (name) that generated this process
string Process::User() {
    return user;
}

// Return the age of this process (in seconds)
long int Process::UpTime() {
    return LinuxParser::UpTime(pid);
}

// Overload the "greater than" comparison operator for Process objects
bool Process::operator>(Process const& a) const {
    return cpu > a.cpu;
}
