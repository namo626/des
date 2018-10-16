/*
 * Interface for configuration program
 *
 * Created by Axel Berglund and Chayanon Wichitrnithed
 */

#ifndef SRC_CONFIG_H_
#define SRC_CONFIG_H_

/*
 * Reads config file and runs simulation
 *
 * @param time the simulation time the simulator should run
 * @param configfile the name of the configuration file
 * @param outfile the name of the output file
 * @return 0 if successful, 1 otherwise
 */
int config(double time, char* configfile, char* outfile);

#endif /* SRC_CONFIG_H_ */
