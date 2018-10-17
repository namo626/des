/*
 * Interface of the simulation library to be used in the configuration program
 * Requirements:
 *   - Functions to create each component
 *   - A function to run the simulation network
 *   - Functions to extract information from the network
 *
 * Created by Axel Berglund and Chayanon Wichitrnithed
 */


/* Initialize the array containing the components
 */
void initialize(int numComponents);

/* Add a Generator component
 */
void addGen(int id, double avgArrivalTime, int outputID);

/* Add an Exit component
 */
void addExit(int id);

/* Add a Queueing Station component
 */
void addStation(int id, double avgServeTime, int outputID);

/* Add a Fork component
 */
void addFork(int id, int count, double* probabilities, int* outputIDs);

/* Run the simulation, modifying state
 */
void runSim(double time);

/* Print to statistics of the global state (overwriting the file)
 * Number of enters and exits, max, min, avg total waiting times of all customers, and
 * number of components
 */
void networkReport(char* filename);

/* Print the statistics of a chosen component (appending to the file)
 * This only prints statistics for Station and Exit components, and does nothing
 * when the given component is of any other type.
 */
void printReport(int id, char* filename);

/* Free the whole simulation
 */
void freeSim();
