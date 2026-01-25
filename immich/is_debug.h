#ifndef IS_DEBUG_H
#define IS_DEBUG_H

#include <fstream>

static bool IS_DEBUG = []() {
  std::ifstream f("/proc/self/status");
  std::string line;
  while (std::getline(f, line)) {
    if (line.rfind("TracerPid:", 0) == 0) {
      int pid = std::stoi(line.substr(10));
      return pid != 0;
    }
  }
  return false;
};

#endif // IS_DEBUG_H
