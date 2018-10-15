#include "Sim.h"

int main() {
  initialize(6);

  addGen(0, 15, 1);
  addStation(1, 3.0, 1);
  double chances[4] = {0.3, 0.2, 0.4, 0.1};
  int outports[4] = {3, 4, 5, 1};
  addFork(2, 4, chances, outports);
  addStation(3, 7.0, 1);
  addStation(4, 5.0, 1);
  addExit(5);

  runSim(70);

  printReport(2);
  printReport(1);
  networkReport();

  return 0;
}
