#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>

using std::string;

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  unsigned long currIdle = 0;
  unsigned long currTotal = 0;
  unsigned long prevIdle;
  unsigned long prevTotal;
};

#endif
