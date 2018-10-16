#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "Internal.h"
#include "Queue.h"
#include "Sim.h"

#define TRUE 0
#define FALSE 1

/***********************************************************************/
/* Auxiliary functions */

double urand() {
  return (double)rand() / (double)((unsigned)RAND_MAX + 1);
}

double randexp(double mean) {
  return -mean * (log(1.0-urand()));
}

/***********************************************************************/

// Component is a general type for all components
typedef struct Component {
  char* type;
  void* content;
} Component;

// Make a general component to hold a specific component
Component* createComp(char* t, void* c) {
  Component* comp = malloc(sizeof(Component));
  if (comp == NULL) {
    printf("Could not create a Component\n");
    return NULL;
  }
  comp->type = t;
  comp->content = c;
  return comp;
}

// Network contains an array of Components
typedef struct Network {
  int num_components; // should be fixed
  Component** components;
  CustList* entered; // a list containing all the customers who have entered
  int totalExits; // count of TOTAL exits
} Network;


// Customer type
struct Customer {
  double createTime; // time at which the customer was generated
  double totalWait; // total time of waiting at the queues
  char* name;

  double tempArrTime;
};

Customer* mkCustomer(char* name, double time) {
  Customer* customer = malloc(sizeof(Customer));
  if (customer == NULL) {
    printf("Could not create a Customer\n");
  }
  customer->createTime = time;
  customer->totalWait = 0;
  customer->tempArrTime = 0;
  customer->name = name;
  return customer;
}

double totalWait(Customer* cust) {
  return cust->totalWait;
}

/*************************************************************/
/* Global state variables and modifying functions */

// global network
Network* NETWORK = NULL;

// initialize the component array of the network
void initialize(int amount) {
  Component** comps = malloc(amount * sizeof(Component*));
  NETWORK = malloc(sizeof(Network));
  NETWORK->num_components = amount;
  NETWORK->components = comps;
  NETWORK->entered = mkCustList();

  // initialize the event priority queue
  initFES();

}

// add a component to the network
void addToNetwork(int id, Component* comp) {
  NETWORK->components[id] = comp;
}

Component* getFromNetwork(int id) {
  return NETWORK->components[id];
}

// add an entering customer
void recordEnter(Customer* customer) {
  insertCust(NETWORK->entered, customer);
}

void incTotalExit() {
  NETWORK->totalExits++;
}

void networkReport(char* filename) {
  FILE* f = fopen(filename, "w");
  if (f == NULL) {
    printf("Creating a new file\n");
  }
  fprintf(f, "Stats for the whole network\n");
  fprintf(f, "  No. of components in system: %d\n", NETWORK->num_components);
  fprintf(f, "  No. of customers entered: %d\n", custCount(NETWORK->entered));
  fprintf(f, "  No. of customers exited: %d\n", NETWORK->totalExits);
  fprintf(f, "  Max total wait time among customers: %.1f\n", maxWaitTime(NETWORK->entered));
  fprintf(f, "  Min total wait time among customers: %.1f\n", minWaitTime(NETWORK->entered));
  fprintf(f, "  Avg total wait time among customers: %.1f\n", avgWaitTime(NETWORK->entered));

  fprintf(f, "\n");
  fclose(f);
}

/*************************************************************/
/* Exit component */
typedef struct Exit {
  int exited;
  double totalTime;
  double minTime;
  double maxTime;
} Exit;

void addExit(int id) {
  Exit* exit = malloc(sizeof(Exit));
  exit->exited = 0;
  exit->totalTime = 0;
  exit->minTime = -1;
  exit->maxTime = 0;

  Component* comp = createComp("E", exit);
  addToNetwork(id, comp);
}

// add an exiting customer
void recordExit(Exit* exit, double time, Customer* c) {
  exit->exited++;
  incTotalExit();
  double duration = time - (c->createTime);
  exit->totalTime = exit->totalTime + duration;

  if (exit->minTime < 0) {
    exit->minTime = duration;
  }
  else if (duration < exit->minTime) {
    exit->minTime = duration;
  }
  else if (duration > exit->maxTime) {
    exit->maxTime = duration;
  }

  printf("  One customer has exited\n");
}

double avgDuration(Exit* exit) {
  if (exit->exited == 0) {
    return 0.0;
  }

  return (exit->totalTime) / (exit->exited);
}

void exitReport(Exit* exit, int id, char* filename) {
  FILE* f = fopen(filename, "a");
  if (f == NULL) {
    printf("Creating a new file\n");
  }
  fprintf(f, "Stats of Exit (id = %d)\n", id);
  fprintf(f, "  Number of customers exited: %d\n", exit->exited);
  fprintf(f, "  Min time in the system: %.1f\n", exit->minTime);
  fprintf(f, "  Max time in the system: %.1f\n", exit->maxTime);
  fprintf(f, "  Avg time in the system: %.1f\n", avgDuration(exit));
  fprintf(f, "\n");
  fclose(f);
}


/*************************************************************/
/* Station component and its functions */

typedef struct Station {
  Queue* queue;
  double avgServiceTime; // given parameter
  double totalWait; // how long are customers in a line
  int inLine; // total number of customers that had to wait in the line
  int outport;
} Station;

void addStation(int id, double param, int outputID) {
  Station* station = malloc(sizeof(Station));

  if (station == NULL) {
    printf("Could not create a Station\n");
  }

  station->avgServiceTime = param;
  station->totalWait = 0;
  station->queue = mkQueue();
  station->outport = outputID;

  Component* comp = createComp("Q", station);
  addToNetwork(id, comp);
}

// Return a randomized service time
double getServiceTime(Station* station) {
  double meanTime = station->avgServiceTime;
  return randexp(meanTime);
}

// Draw a random wait time of the station
double getAvgWaitTime(Station* station) {
  if (station->inLine == 0) {
    return 0.0;
  }
  return (station->totalWait) / (station->inLine);
}

void stationReport(Station* station, int id, char* filename) {
  FILE* f = fopen(filename, "a");
  if (f == NULL) {
    printf("Creating new file\n");
  }
  fprintf(f, "Stats for Station (id = %d)\n", id);
  fprintf(f, "  Total number of instances of waiting in line: %d\n",
         station->inLine);
  fprintf(f, "  Average line wait time: %.1f\n", getAvgWaitTime(station));
  fprintf(f, "\n");

  fclose(f);
}

void incWaitTime(Station* station, double wt) {
  station->totalWait = station->totalWait + wt;
}

void incInLine(Station* station) {
  station->inLine++;
}


int isEmpty(Station* station) {
  return isEmptyQ(station->queue);
}

void addToLine(Station* station, Customer* customer) {
  enqueue(station->queue, customer);
}

void* removeFromLine(Station* station) {
  return dequeue(station->queue);
}

void* nextInLine(Station* station) {
  return viewHead(station->queue);
}


/***********************************************************************/
/* Fork component */
typedef struct Fork {
  int* outports;
  double* chances;
  int count;
} Fork;

void addFork(int id, int count, double* chances, int* outports) {
  Fork* fork = malloc(sizeof(Fork));
  if (fork == NULL) {
    printf("Could not create a Fork\n");
    return;
  }
  fork->chances = chances;
  fork->outports = outports;
  fork->count = count;

  Component* comp = createComp("F", fork);
  addToNetwork(id, comp);
}

// https://stackoverflow.com/questions/4265988/generate-random-numbers-with-a-given-numerical-distribution
int randomizePort(Fork* fork) {
  double* chances = fork->chances;
  int* ports = fork->outports;
  double prob = urand();
  double s = 0;
  for (int i = 0; i < fork->count; i++) {
    s = s + chances[i];
    if (s >= prob) {
      return ports[i];
    }
  }

  // in case of inaccuracies
  return ports[fork->count - 1];
}

/*************************************************************/
/* Event types */

// generic event
typedef struct Event {
  char* type; // event type: arrival, departure, etc.
  double timestamp;
  void* event; // pointer to the actual event
} Event;

typedef struct Arrival {
  int destID;
  Customer* customer;
} Arrival;

typedef struct Departure {
  int locationID;
  // Customer* customer;
} Departure;

typedef struct Generation {
  int genID;
  int destID;
} Generation;

Event* mkEvent(double time, char* type, void* ev) {
  Event* new = malloc(sizeof(Event));
  if (new == NULL) {
    printf("Could not create an Event\n");
    return NULL;
  }
  new->timestamp = time;
  new->type = type;
  new->event = ev;
  return new;
}


Event* mkArrival(double time, int outputID, Customer* c) {
  Arrival* arrival = malloc(sizeof(Arrival));
  if (arrival == NULL) {
    printf("Could not create an Arrival event\n");
    return NULL;
  }
  arrival->customer = c;
  arrival->destID = outputID;

  Event* event = mkEvent(time, "A", arrival);

  return event;
}

Event* mkDeparture(double t, int location) {
  Departure* departure = malloc(sizeof(Departure));
  if (departure == NULL) {
    printf("Could not create a departure event\n");
    return NULL;
  }
  departure->locationID = location;

  Event* event = mkEvent(t, "D", departure);

  return event;
}

Event* mkGeneration(double t, int genID, int destID) {
  Generation* gen = malloc(sizeof(Generation));
  if (gen == NULL) {
    printf("Could not create a Generator event\n");
    return NULL;
  }
  gen->genID = genID;
  gen->destID = destID;

  Event* event = mkEvent(t, "M", gen);

  return event;
}


/***************************************************************/
/* Generator component and its functions */

typedef struct Gen {
  int outport; // array of IDs of the connected components
  double avgArrivalTime; // parameter: average arrival time
} Gen;

// Return a random arrival time (duration)
double getGenTime(Gen* gen) {
  double meanTime = gen->avgArrivalTime;
  //  return randexp(meanTime);
  return meanTime;
}

// create and add a generator component to the global network
void addGen(int id, double p, int outputID) {
  Gen* gen = malloc(sizeof(Gen));

  if (gen == NULL) {
    printf("Could not create a Generator\n");
    return;
  }

  gen->outport = outputID;
  gen->avgArrivalTime = p;

  Component* comp = createComp("G", gen);
  addToNetwork(id, comp);

  // initialize the first generation
  double now = currentTime(); // should be 0.0
  double timestamp = now + getGenTime(gen);
  Event* ev = mkGeneration(timestamp, id, outputID);
  schedule(ev, timestamp);

}


/***************************************************************/

void handleGeneration(Generation* gent) {
  double now = currentTime();

  // make a new customer now
  Customer* c = mkCustomer("", now);
  recordEnter(c);
  printf("  A customer has been created\n");

  // schedule the arrival event
  schedule(mkArrival(now, gent->destID, c), now);

  // schedule next generation event
  Component* comp = getFromNetwork(gent->genID);
  Gen* gen = comp->content;
  double t = getGenTime(gen);
  schedule(mkGeneration(now+t, gent->genID, gent->destID), now+t);
}

void handleArrival(Arrival* arrival) {
  double now = currentTime();

  // component that the arrival is sent to
  Component* dest = getFromNetwork(arrival->destID);
  Customer* customer = arrival->customer;

  // dispatching on component type and handle accordingly
  if (dest->type == "E") {
    Exit* exit = dest->content;
    recordExit(exit, now, customer);
  }
  else if (dest->type == "Q") {
    printf("  Customer %s arrived at station %d\n", customer->name, arrival->destID);
    Station* station = dest->content;

    if (isEmpty(station) == TRUE) {
      double leaveTime = getServiceTime(station) + now;
      schedule(mkDeparture(leaveTime, arrival->destID),
               leaveTime);
    }
    else {
      // mark the time that the customer starts to wait (temporary)
      customer->tempArrTime = now;
    }
    addToLine(station, customer);
  }
  else if (dest->type == "F") {
    Fork* fork = dest->content;
    int port = randomizePort(fork);
    schedule(mkArrival(now, port, customer), now);
    printf("  Customer %s arrived at fork, going to port %d\n", customer->name, port);
  }
}

void handleDeparture(Departure* departure) {
  double now = currentTime();

  // dispatching on component type
  Component* loc = getFromNetwork(departure->locationID);

  // can only depart from a Station
  if (loc->type == "Q") {
    Station* station = loc->content;
    Customer* customer = removeFromLine(station);

    // customer instantly arrives at the next component
    int dest = station->outport;
    schedule(mkArrival(now, dest, customer), now);

    // if station is still not empty, process the next customer
    if (isEmpty(station) == FALSE) {
      Customer* nextCust = nextInLine(station);
      double leaveTime = getServiceTime(station) + now;

      // customer finishes waiting in queue (line)
      double waitTime = now - (nextCust->tempArrTime);
      nextCust->totalWait = nextCust->totalWait + waitTime;
      printf("  Next customer has waited for %.1f\n", nextCust->totalWait);
      incWaitTime(station, waitTime);
      incInLine(station);

      schedule(mkDeparture(leaveTime, departure->locationID),
               leaveTime);
    }
    printf("  Customer %s departed\n", customer->name);
  }
  else {
    printf("Error: customer not departing from a Station\n");
  }
}

// handler on generic events
void handleEvent(void* e) {
  Event* event = (Event*) e;

  if (event->type == "A") {
    Arrival* arrival = (Arrival*) event->event;
    handleArrival(arrival);
  }
  else if (event->type == "D") {
    Departure* departure = (Departure*) event->event;
    handleDeparture(departure);
  }
  else if (event->type == "M") {
    Generation* gent = event->event;
    handleGeneration(gent);
  }

  free(event->event);
  free(event);
}

/***********************************************************************/
/* Printing statistics */

void printReport(int id, char* filename) {

  Component* comp = getFromNetwork(id);

  if (comp->type == "Q") {
    Station* st = (Station*) comp->content;
    stationReport(st, id, filename);
  }
  else if (comp->type == "E") {
    Exit* exit = comp->content;
    exitReport(exit, id, filename);
  }

}
