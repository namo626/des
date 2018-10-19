#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "Config.h"
#include "Sim.h"

/*
 * Functions
 */
void *read_file(FILE* ifp, int num_components);
void *read_line(char* buffer, int num_components, int has_id[], int is_output[],
		int is_generator[]);
int get_id(char* buffer);
char get_component_type(char* buffer);
void *create_g_q(int id, int gen, int is_output[], int num_components);
void *create_fork(int id, int is_output[], int num_components);

/*
 * Reads config file and runs simulation
 */
int config(double time, char* configfile, char* outfile) {
	FILE* ifp;
	ifp = fopen(configfile, "r");
	if (ifp == NULL) {
		printf("Could not open file");
		return 1;
	}

	int max_buf = 1000;
	char *buffer = malloc(sizeof(char) * max_buf);
	if (buffer == NULL) {
		//Malloc failed
		printf("Failed to allocate memory");
		fclose(ifp);
		return 1;
	}
	memset(buffer, 0, 1000);

	//Get first line of input file, which is number of components
	int num_components;
	int buf_length = 0;
	char ch;
	while ((ch != '\n') && (ch != EOF)) {
		if (buf_length == max_buf) {
			max_buf *= 2;
			buffer = realloc(buffer, max_buf);
			if (buffer == NULL) {
				//Realloc failed
				printf("Failed to allocate memory (reallocation)");
				fclose(ifp);
				return 1;
			}
		}
		ch = getc(ifp);
		buffer[buf_length] = ch;
		buf_length++;
	}
	int i = 0;
	while (i < strlen(buffer) - 2) {
		if (!isdigit(buffer[i])) {
			printf(
					"ERROR: The first line of the config file is not an integer\n");
			if (buffer != NULL) {
				freeSim();
				free(buffer);
			}
			return 1;
		}
		i++;
	}
	num_components = strtol(buffer, (char **) NULL, 10);
	initialize(num_components);
	printf("Simulator initialized...\nNumber of components: %d\n\n",
			num_components);

	//Read the rest of the file
	if (!(read_file(ifp, num_components) == NULL)) {
		//All components added successfully, run simulation
		printf("Running simulation...\n");
		runSim(time);
		printf("Finished running simulation...\n");
		networkReport(outfile);
		for (int i = 0; i < num_components; i++) {
			printReport(i, outfile);
			printf("Printing report for component with ID: %d\n", i);
		}
		if (buffer != NULL) {
			free(buffer);
		}
		freeSim();
		fclose(ifp);
		return 0;
	} else {
		//There was an error
		if (buffer != NULL) {
			free(buffer);
		}
		freeSim();
		fclose(ifp);
		return 1;
	}
}

/*
 * Reads a configuration file and creates components accordingly
 */
void* read_file(FILE* ifp, int num_components) {
	//Arrays for error checking
	int has_id[num_components];
	int is_output[num_components];
	int is_generator[num_components];
	for (int i = 0; i < num_components; i++) {
		has_id[i] = 0;
		is_output[i] = 0;
		is_generator[i] = 0;
	}

	int max_buf = 1000;
	char *buffer = malloc(sizeof(char) * max_buf);
	if (buffer == NULL) {
		//Malloc failed
		printf("Failed to allocate memory");
		fclose(ifp);
		return NULL;
	}
	int buf_length = 0;
	char ch;
	while (ch != EOF) {
		ch = '\0';

		//Read next line, store in buffer
		while (ch != '\n' && ch != EOF) {
			if (buf_length == max_buf) {
				max_buf *= 2;
				buffer = realloc(buffer, max_buf);
				if (buffer == NULL) {
					//Realloc failed
					printf("Failed to allocate memory (reallocation)");
					fclose(ifp);
					return NULL;
				}
			}
			ch = getc(ifp);
			buffer[buf_length] = ch;
			buf_length++;
		}
		buffer[buf_length - 1] = '\0';

		//Read line of buffer
		if (read_line(buffer, num_components, has_id, is_output,
				is_generator) == NULL) {
			//There was an error
			if (buffer != NULL) {
				free(buffer);
			}
			return NULL;
		}

		//New line/buffer
		buf_length = 0;
	}
	if (buffer != NULL) {
		free(buffer);
	}
	return ((void*) 1);
}

/*
 * Reads a line, creates the component specified on that line
 */
void *read_line(char* buffer, int num_components, int has_id[], int is_output[],
		int is_generator[]) {
	if (buffer[0] == '\0') {
		//Empty line, skip
		return ((void*) 1);
	}
	char *split = strtok(buffer, " ");
	int id;
	char component_type;
	int count = 0;
	//Get ID and component type
	while (split != NULL) {
		if (count == 0) {
			//ID
			id = get_id(split);
			if (id == -1) {
				return NULL;
			}
			if (id >= num_components) {
				printf("ERROR: The ID of one of the components is too large\n");
				return NULL;
			}
			if (has_id[id]) {
				printf(
						"ERROR: The configuration file contains two identical IDs\n");
				return NULL;
			}
			has_id[id] = 1;
		}
		if (count == 1) {
			//Component type
			component_type = get_component_type(split);
			if (component_type == '\0') {
				return NULL;
			}

		}
		count++;
		if (count < 2) {
			//Still on component type or ID
			split = strtok(NULL, " ");
		} else {
			//We now have component type and ID
			split = NULL;
		}
	}
	if (count != 2) {
		printf("ERROR: Missing arguments to create component\n");
		return NULL;
	}

	switch (component_type) {
	case 'E':
		if (strtok(NULL, " ") != NULL) {
			printf("ERROR: Exit component does not allow arguments\n");
			return NULL;
		}
		printf("ID: %d\n", id);
		addExit(id);
		printf("Created exit component\n\n");
		break;
	case 'G':
		if (create_g_q(id, 1, is_output, num_components) == NULL) {
			return NULL;
		}
		is_generator[id] = 1;
		break;
	case 'Q':
		if (create_g_q(id, 0, is_output, num_components) == NULL) {
			return NULL;
		}
		break;
	case 'F':
		if (create_fork(id, is_output, num_components) == NULL) {
			return NULL;
		}
		break;
	default:
		printf("ERROR: Something went wrong\n");
		return NULL;
		break;
	}
	//Check output IDs
	for (int i = 0; i < num_components; i++) {
		if (is_output[i]) {
			if (is_generator[i]) {
				printf("ERROR: Output ID can't be the ID of a generator\n");
				return NULL;
			}
		}
	}
	return ((void*) 1);
}

/*
 * Creates a fork component
 */
void* create_fork(int id, int is_output[], int num_components) {
	char *split = strtok(NULL, " ");
	int i = 0;
	int num_ports;
	while (i < strlen(split)) {
		if (!isdigit(split[i])) {
			printf("ERROR: Number of output ports must be an integer\n");
			return NULL;
		}
		i++;
	}
	num_ports = strtol(split, (char**) NULL, 10);

	//Probabilities
	double* probabilities = malloc(num_ports * sizeof(double));
	if (probabilities == NULL) {
		printf("Failed to allocate memory");
		return NULL;
	}

	for (int count = 0; count < num_ports; count++) {
		int decimal = 0;
		split = strtok(NULL, " ");
		if (split == NULL) {
			printf("ERROR: Not enough arguments for fork\n");
			freeSim();
			return NULL;
		}
		int i = 0;
		while (i < strlen(split)) {
			if (!isdigit(split[i])) {
				if (!decimal && split[i] == '.') {
					decimal = 1;
				} else {
					printf("ERROR: Probabilities for fork must be a number\n");
					freeSim();
					return NULL;
				}
			}
			i++;
		}
		if (split[i - 1] == '.') {
			//Nothing comes after the decimal point
			printf("ERROR: Probabilities of fork are not valid\n");
			return NULL;
		}
		probabilities[count] = strtod(split, (char**) NULL);
	}

	//Output ids
	int *output_ids = malloc(num_ports * sizeof(int));
	if (output_ids == NULL) {
		printf("Failed to allocate memory");
		return NULL;
	}

	for (int count = 0; count < num_ports; count++) {
		split = strtok(NULL, " ");
		if (split == NULL) {
			printf("ERROR: Not enough arguments for fork\n");
			return NULL;
		}
		int i = 0;
		while (i < strlen(split) - 1) {
			if (!isdigit(split[i])) {
				printf("ERROR: Output ports for fork must be a number\n");
				return NULL;
			}
			i++;
		}
		int output_id = strtol(split, (char**) NULL, 10);
		if (output_id >= num_components) {
			printf("ERROR: Output ID of fork does not exist\n");
			return NULL;
		}
		output_ids[count] = output_id;
		is_output[output_id] = 1;
	}
	if (strtok(NULL, " ") != NULL) {
		printf("ERROR: Too many arguments for fork component\n");
		return NULL;
	}
	//Check that probabilities add up to 1
	double sum = 0.0;
	for (int i = 0; i < num_ports; i++) {
		sum += probabilities[i];
	}
	if (sum != 1) {
		printf("ERROR: Probabilities of fork must add up to 1\n");
		return NULL;
	}

	printf("ID: %d\n", id);
	printf("Number of output ports: %d\n", num_ports);
	for (int count = 0; count < num_ports; count++) {
		printf("Probability %d: %f\n", count, probabilities[count]);
		printf("Output id %d: %d\n", count, output_ids[count]);
	}

	addFork(id, num_ports, probabilities, output_ids);
	printf("Created fork component\n\n");
	return ((void*) 1);
}

/*
 * Creates generator or queuing station, int gen indicates which one.
 * 1 = generator, 0 = queuing station
 */
void* create_g_q(int id, int gen, int is_output[], int num_components) {
	char *split = strtok(NULL, " ");
	if (split == NULL) {
		if (gen) {
			printf(
					"ERROR: Generator needs an average interarrival time and an output ID\n");
		} else {
			printf(
					"ERROR: Queuing station needs an average service time and an output ID\n");
		}
		return NULL;
	}
	int i = 0;
	int decimal = 0;
	while (i < strlen(split)) {
		if (!isdigit(split[i])) {
			if (!decimal && split[i] == '.') {
				decimal = 1;
			} else {
				if (gen) {
					printf(
							"ERROR: Interarrival time of generator should be a number\n");
				} else {
					printf(
							"ERROR: Service time of queuing station should be a number\n");
				}
				return NULL;
			}
		}
		i++;
	}
	if (split[i - 1] == '.') {
		//Nothing comes after the decimal point
		if (gen) {
			printf("ERROR: Interarrival time of generator is incorrect\n");
		} else {
			printf("ERROR: Service time of queuing station is incorrect\n");
		}
		return NULL;
	}
	double time = strtod(split, (char **) NULL);
	split = strtok(NULL, " ");
	if (split == NULL) {
		if (gen) {
			printf(
					"ERROR: Generator needs an average interarrival time and an output ID\n");
		} else {
			printf(
					"ERROR: Queuing station needs an average service time and an output ID\n");
		}
		return NULL;
	}
	i = 0;
	int output_id;
	while (i < strlen(split) - 1) {
		if (!isdigit(split[i])) {
			if (gen) {
				printf("ERROR: Output ID of generator must be a number\n");
			} else {
				printf(
						"ERROR: Output ID of queuing station must be a number\n");
			}
			return NULL;
		}
		i++;
	}
	if (strtok(NULL, " ") != NULL) {
		if (gen) {
			printf("ERROR: Too many arguments for generator component\n");
			return NULL;
		} else {
			printf("ERROR: Too many arguments for queuing station component\n");
			return NULL;
		}
	}
	output_id = strtol(split, (char**) NULL, 10);
	if (output_id >= num_components) {
		if (gen) {
			printf("ERROR: Output ID of generator does not exist\n");
			return NULL;
		} else {
			printf("ERROR: Output ID of queuing station does not exist\n");
			return NULL;
		}
	}
	is_output[output_id] = 1;

	printf("ID: %d\n", id);
	if (gen) {
		printf("Component type: G\n");
	} else {
		printf("Component type: Q\n");
	}
	printf("Time: %f\n", time);
	printf("Output ID: %d\n", output_id);
	if (gen) {
		addGen(id, time, output_id);
	} else {
		addStation(id, time, output_id);
	}
	if (gen) {
		printf("Created generator component\n\n");
	} else {
		printf("Created queuing station component\n\n");
	}
	return ((void*) 1);
}

/*
 * Gets the ID of the component by reading first part of line
 */
int get_id(char* buffer) {
	int i = 0;
	while (i < strlen(buffer)) {
		if (!isdigit(buffer[i])) {
			printf("ERROR: Each line must start with component ID\n");
			return -1;
		}
		i++;
	}
	return strtol(buffer, (char **) NULL, 10);
}

/*
 * Gets the component type of the line
 */
char get_component_type(char* buffer) {
	if (strlen(buffer) - 1 > 1) {
		printf("ERROR: Component type should only be one letter\n");
		return '\0';
	}
	char ch = buffer[0];
	if (ch == 'G' || ch == 'E' || ch == 'Q' || ch == 'F') {
		return ch;
	} else {
		printf("ERROR: Component type does not exist (%c)\n", ch);
		return '\0';
	}
}
