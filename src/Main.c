#include "Sim.h"

int main() {
  initialize(10);

  addGen(0, 15, 1);
  addExit(1);
  addStation(2, 15, 1);

  double chances[2] = {0.2, 0.8};
  int outports[2] = {1, 2};
  addFork(3, chances, outports);

  addGen(4, 10, 1);

  runSim(46);

  printReport(2);
  printReport(1);
  networkReport();

  return 0;
}
