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
void addFork(int id, double* probabilities, int* outputIDs);

/* Run the simulation, modifying state
 */
void runSim();
