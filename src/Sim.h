/*
 * Interface of the simulation library to be used in the configuration program
 * Requirements:
 *   - Functions to create each component
 *   - A function to run the simulation network
 *   - Functions to extract information from the network
 *
 * Created by Axel Berglund and Chayanon Wichitrnithed
 */

typedef struct Gen Gen;
typedef struct Exit Exit;
typedef struct Station Station;
typedef struct Fork Fork;

void addGen(double avgArrivalTime, int outputID);

void addExit();

void addStation(double avgServeTime, int outputID);

void addFork(double* probabilities, int* outputIDs);

void runSim();
