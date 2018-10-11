#include <stdlib.h>
#include <stdio.h>
#include "Internal.h"
#include "Queue.h"

#define TRUE 0
#define FALSE 1

// COmponent is a general type for all components
typedef struct Component {
  char* type;
  void* content;
} Component;

// Make a general component to hold a specific component
Component* createComp(char* t, void* c) {
  Component* comp = malloc(sizeof(Component));
  // check if comp is NULL
  comp->type = t;
  comp->content = c;
  return comp;
}

// Network contains an array of Components
typedef struct Network {
  int num_components; // should be fixed
  Component** components;
  int entered; // customers who have entered
} Network;


// Customer type
typedef struct Customer {
  double totalTime; // total time spent in the system
  double totalWait; // total time of waiting at the queues
  char* name;
  double tempArrTime;
} Customer;

Customer* mkCustomer(char* name) {
  Customer* customer = malloc(sizeof(Customer));
  customer->totalTime = 0;
  customer->totalWait = 0;
  customer->tempArrTime = 0;
  customer->name = name;
  return customer;
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
  NETWORK->entered = 0;

}

// add a component to the network
void addToNetwork(int id, Component* comp) {
  NETWORK->components[id] = comp;
}

Component* getFromNetwork(int id) {
  return NETWORK->components[id];
}


// add an entering customer
void recordEnter(Customer* customer, double time) {
  NETWORK->entered++;
}


/***************************************************************/
/* Generator component and its functions */

typedef struct Gen {
  int outport; // array of IDs of the connected components
  double avgArrivalTime; // parameter: average arrival time
} Gen;

// create and add a generator component to the global network
void addGen(int id, double p, int outputID) {
  Gen* gen = malloc(sizeof(Gen));

  if (gen == NULL) {
    printf("Not enough memory");
  }

  gen->outport = outputID;
  gen->avgArrivalTime = p;

  Component* comp = createComp("G", gen);
  addToNetwork(id, comp);

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
void recordExit(Exit* exit, double time) {
  exit->exited++;
  exit->totalTime = exit->totalTime + time;

  if (exit->minTime < 0) {
    exit->minTime = time;
  }
  else if (time < exit->minTime) {
    exit->minTime = time;
  }
  else if (time > exit->maxTime) {
    exit->maxTime = time;
  }

  printf("  One customer has exited\n");
}

double avgDuration(Exit* exit) {
  if (exit->exited == 0) {
    return 0.0;
  }

  return (exit->totalTime) / (exit->exited);
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
    printf("Not enough memory");
  }

  station->avgServiceTime = param;
  station->totalWait = 0;
  station->queue = mkQueue();
  station->outport = outputID;

  Component* comp = createComp("Q", station);
  addToNetwork(id, comp);
}

double getServiceTime(Station* station) {
  return station->avgServiceTime;
}

// Draw a random wait time of the station
double getAvgWaitTime(Station* station) {
  return (station->totalWait) / (station->inLine);
}

void incWaitTime(Station* station, double wt) {
  station->totalWait = station->totalWait + wt;
}


int isEmpty(Station* station) {
  return isEmptyQ(station->queue);
}

void addToLine(Station* station, Customer* customer) {
  enqueue(station->queue, customer);
  station->inLine++;
}

void* removeFromLine(Station* station) {
  return dequeue(station->queue);
}


/***********************************************************************
/* Fork component */
typedef struct Fork {
  int* outports;
  double* chances;
} Fork;

void addFork(int id, double* chances, int* outports) {
  Fork* fork = malloc(sizeof(Fork));
  fork->chances = chances;
  fork->outports = outports;

  Component* comp = createComp("F", fork);
  addToNetwork(id, comp);
}

int randomizePort(Fork* fork) {
  return fork->outports[1];
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
  Customer* customer;
} Departure;


Event* mkArrival(double time, int outputID, Customer* c) {
  Arrival* arrival = malloc(sizeof(Arrival));
  Event* event = malloc(sizeof(Event));

  arrival->customer = c;
  arrival->destID = outputID;

  event->event = arrival;
  event->type = "A";
  event->timestamp = time;

  return event;
}

Event* mkDeparture(double t, int location, Customer* c) {
  Departure* departure = malloc(sizeof(Departure));
  Event* event = malloc(sizeof(Event));

  departure->customer = c;
  departure->locationID = location;

  event->timestamp = t;
  event->type = "D";
  event->event = departure;

  return event;
}

void handleArrival(Arrival* arrival) {
  double now = currentTime();

  // component that the arrival is sent to
  Component* dest = getFromNetwork(arrival->destID);
  Customer* customer = arrival->customer;

  // dispatching on component type and handle accordingly
  if (dest->type == "E") {
    Exit* exit = dest->content;
    recordExit(exit, now);
  }
  else if (dest->type == "Q") {
    printf("  Customer %s arrived\n", customer->name);
    Station* station = dest->content;

    if (isEmpty(station) == TRUE) {
      double leaveTime = getServiceTime(station) + now;
      schedule(mkDeparture(leaveTime, arrival->destID, customer),
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
      double leaveTime = getServiceTime(station) + now;

      // customer finishes waiting in queue (line)
      double waitTime = now - (customer->tempArrTime);
      customer->totalWait = customer->totalWait + waitTime;
      incWaitTime(station, waitTime);

      schedule(mkDeparture(leaveTime, departure->locationID, customer),
               leaveTime);
    }
    printf("  Customer %s departed\n", customer->name);
  }
  else {
    printf("Error: customer not departing from a Station\n");
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

  free(event->event);
  free(event);
}

int main() {
  initialize(10);
  initFES();

  addGen(0, 15, 3);
  addExit(1);
  addStation(2, 15, 1);

  double chances[2] = {0.2, 0.8};
  int outports[2] = {1, 2};
  addFork(3, chances, outports);

  Event* ev = mkArrival(15, 3, mkCustomer("A"));
  schedule(ev, ev->timestamp);

  Event* ev2 = mkArrival(22, 2, mkCustomer("B"));
  schedule(ev2, ev2->timestamp);

  Event* ev3 = mkArrival(24, 2, mkCustomer("C"));
  schedule(ev3, ev3->timestamp);

  runSim(100);
}
