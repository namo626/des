#include <stdlib.h>
#include <stdio.h>
#include "Internal.h"
#include "PrioQ.h"

// global schedule
PrioQ* FES;
// global clock
double NOW = 0.0;

double currentTime() {
  return NOW;
}

void schedule(void* event, double timestamp) {
  PQ_insert(FES, timestamp, event);
}

void* nextEvent() {
  return PQ_delete(FES);
}

/*************************************************************/
/* Running the network simulation */

void runSim(double time) {
  FES = PQ_create();
  /* // run the main generator of the network to generate all the arrivals */
  /* Gen* gen = findGen(); */
  /* double genTime = 0; */
  /* double arrivalTime; */
  /* while (genTime < time) { */
  /*   arrivalTime = getAvgTime(gen); */
  /*   Event* ev = mkArrival(genTime + arrivalTime); */
  /*   // add event to FEL */
  /*   schedule(ev); */
  /*   genTime = genTime + arrivalTime; */
  /* } */

  /* // main event processing loop for events from FEL */
  /* while (NOW < time) { */
  /*   // remove event from FEL */
  /*   Event* event = nextEvent(); */
  /*   NOW = getTimeStamp(event); */
  /*   // handleEvent may schedule a new event, for example */
  /*   handleEvent(event); */
  /* } */
}
