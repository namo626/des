#include <stdlib.h>
#include <stdio.h>
#include "Internal.h"
#include "PrioQ.h"
#include "Sim.h"

// global schedule
PrioQ* FES;
// global clock
double NOW = 0.0;

double currentTime() {
  return NOW;
}

void initFES() {
  FES = PQ_create();
}

void schedule(void* event, double timestamp) {
  PQ_insert(FES, timestamp, event);
}

void* nextEvent(double* holder) {
  return PQ_delete(FES, holder);
}

/*************************************************************/
/* Running the network simulation */

void runSim(double time) {

  // main event processing loop for events from FEL
  double timeHolder;
  void* event;
  while ((event=nextEvent(&timeHolder)) != NULL) {
    // update the clock
    NOW = timeHolder;
    if (NOW > time) break;
    printf("Clock is now %.1f\n", NOW);
    // handleEvent may schedule a new event, for example
    handleEvent(event);
  }
}
