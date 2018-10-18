#include "Sim.h"

// For study() only
#define forks 5
#define drinks 2
#define checkout 2

// Sample config
void sample() {
  initialize(6);
  double chances[4] = {0.3, 0.2, 0.4, 0.1};
  int ports[4] = {3, 4, 5, 1};

  addGen(0, 15.0, 1);
  addStation(1, 20.0, 2);
  addFork(2, 4, chances, ports);
  addStation(3, 7.0, 1);
  addStation(4, 5.0, 1);
  addExit(5);

  runSim(1000);

  networkReport("report.txt");
  printReport(3, "report.txt");
}

// Sample study system
void study() {
  double chances[forks] = {0.1, 0.15, 0.2, 0.25, 0.3};
  int outports[forks] = {3, 4, 5, 6, 7};

  double dp[drinks] = {0.5, 0.5};
  int dout[drinks] = {9, 10};

  double cp[checkout] = {0.95, 0.05};
  int cout[checkout] = {13, 2};

  initialize(15);

  // Generator
  addGen(0, 0.1, 13);
  // Station (tray)
  addStation(1, 0.2, 2);
  // Fork to different restaurants
  addFork(2, forks, chances, outports);
  addStation(3, 6.0, 8);
  addStation(4, 2.5, 8);
  addStation(5, 3.3, 8);
  addStation(6, 2.8, 8);
  addStation(7, 1.5, 8);

  // 2 beverage stations
  addFork(8, drinks, dp, dout);
  addStation(9, 1.0, 11);
  addStation(10, 1.0, 14);

  // checkout station which forks to exit or the restaurant fork
  addStation(11, 1.0, 13);
  addFork(12, checkout, cp, cout);
  addExit(13);

  addStation(14, 1.0, 13);

  runSim(50);

  char* filename = "report.txt";
  networkReport(filename);
  printReport(1, filename);
  printReport(3, filename);
  printReport(13, filename);

}
/*
 int main() {
   sample();
   return 0;
 }
*/
