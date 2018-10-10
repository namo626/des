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
  int exited;  // customers who have exited
  int entered; // customers who have entered
} Network;


// Customer type
typedef struct Customer {
  double totalTime;
} Customer;

Customer* mkCustomer() {
  Customer* customer = malloc(sizeof(Customer));
  customer->totalTime = 0;
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
  NETWORK->exited = 0;

}

// add a component to the network
void addToNetwork(int id, Component* comp) {
  NETWORK->components[id] = comp;
}

Component* getFromNetwork(int id) {
  return NETWORK->components[id];
}

// add an exiting customer
void recordExit(Customer* customer) {
  NETWORK->exited++;
}

// add an entering customer
void recordEnter(Customer* customer) {
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

void addExit(int id) {
  Component* comp = createComp("E", NULL);
  addToNetwork(id, comp);
}


/*************************************************************/
/* Station component and its functions */

typedef struct Station {
  Queue* queue;
  double avgWaitTime; // given parameter
  int outport;
} Station;

void addStation(int id, double param, int outputID) {
  Station* station = malloc(sizeof(Station));

  if (station == NULL) {
    printf("Not enough memory");
  }

  station->avgWaitTime = param;
  station->queue = mkQueue();
  station->outport = outputID;

  Component* comp = createComp("Q", station);
  addToNetwork(id, comp);
}

double getWaitTime(Station* station) {
  // random from a distribution based on the avgWaittime param
  double param = station->avgWaitTime;
  return 1.0;
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


Event* mkArrival(double time, int outputID) {
  Arrival* arrival = malloc(sizeof(Arrival));
  Event* event = malloc(sizeof(Event));

  arrival->customer = mkCustomer();
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

  // component that the arrival is sent to
  Component* dest = getFromNetwork(arrival->destID);
  Customer* customer = arrival->customer;

  // dispatching on component type and handle accordingly
  if (dest->type == "E") {
    recordExit(customer);
  }
  else if (dest->type == "Q") {
    Station* station = dest->content;
    if (isEmpty(station) == TRUE) {
      double waitTime = getWaitTime(station);
      double timestamp = waitTime + currentTime();
      schedule(mkDeparture(timestamp, arrival->destID, customer),
               timestamp);
    }
    addToLine(station, customer);
  }
  else if (dest->type == "F") {
    return;
  }
}

void handleDeparture(Departure* departure) {
  // dispatching on component type
  Component* loc = getFromNetwork(departure->locationID);
  // can only depart from a Station
  if (loc->type == "Q") {
    Station* station = loc->content;
    Customer* customer = removeFromLine(station);
    // get the next destination from the station
    int dest = station->outport;
    schedule(mkArrival(currentTime(), dest), currentTime());

    // if station is still not empty, process the next customer
    if (isEmpty(station) == FALSE) {
      double waitTime = getWaitTime(station);
      double timestamp = currentTime() + waitTime;
      schedule(mkDeparture(timestamp, departure->locationID, customer),
               timestamp);
    }
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
}
