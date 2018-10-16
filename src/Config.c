#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "Config.h"
#include "Sim.h"

/*
 * Functions
 */
int config(double time, char* configfile, char* outfile);
void* read_file(FILE* ifp);
void *read_line(char* buffer);
int get_id(char* buffer);
char get_component_type(char* buffer);
void *create_g_q(int id, int gen);
void *create_fork(int id);

/*
 * Runs simulation
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
		free(ifp);
		return 1;
	}

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
				free(ifp);
				return 1;
			}
		}
		ch = getc(ifp);
		buffer[buf_length] = ch;
		buf_length++;
	}
	int i = 0;
	while (buffer[i] != '\0' && buffer[i] != '\n' && buffer[i] != EOF) {
		if (!isdigit(buffer[i])) {
			printf(
					"Error: The first line of the config file is not an integer");
			fclose(ifp);
			free(ifp);
			return 1;
		}
		i++;
	}
	num_components = strtol(buffer, (char **) NULL, 10);
	initialize(num_components);
	printf("Simulator initialized. Number of components: %d\n\n",
			num_components);

	//Read the rest of the file
	if (!(read_file(ifp) == NULL)) {
		//Success
		return 0;
	} else {
		//There was an error
		return 1;
	}
}

void* read_file(FILE* ifp) {
	int max_buf = 1000;
	char *buffer = malloc(sizeof(char) * max_buf);
	if (buffer == NULL) {
		//Malloc failed
		printf("Failed to allocate memory");
		fclose(ifp);
		free(ifp);
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
					free(ifp);
					return NULL;
				}
			}
			ch = getc(ifp);
			buffer[buf_length] = ch;
			buf_length++;
		}
		buffer[buf_length - 1] = '\0';

		//Read line of buffer
		if (read_line(buffer) == NULL) {
			fclose(ifp);
			free(ifp);
			free(buffer);
			return NULL;
		}

		//New line/buffer
		buf_length = 0;
	}

	fclose(ifp);
	free(ifp);
	free(buffer);
	return ((void*) 1);
}

void *read_line(char* buffer) {
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
			printf("ID: %d\n", id);
		}
		if (count == 1) {
			//Component type
			component_type = get_component_type(split);
			if (component_type == '\0') {
				return NULL;
			}
			printf("Component type: %c\n", component_type);
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
		printf("Missing arguments to create component");
		return NULL;
	}

	switch (component_type) {
	case 'E':
		if (strtok(NULL, " ") != NULL) {
			printf("Exit component does not allow arguments");
			return NULL;
		}
		addExit(id);
		printf("Created exit component\n\n");
		break;
	case 'G':
		if (create_g_q(id, 1) == NULL) {
			return NULL;
		}
		break;
	case 'Q':
		if (create_g_q(id, 0) == NULL) {
			return NULL;
		}
		break;
	case 'F':
		if (create_fork(id) == NULL) {
			return NULL;
		}
		break;
	default:
		printf("Something went wrong");
		return NULL;
		break;
	}

	return ((void*) 1);
}

void* create_fork(int id) {
	char *split = strtok(NULL, " ");
	int i = 0;
	int num_ports;
	while (i < strlen(split)) {
		if (!isdigit(split[i])) {
			printf("Number of output ports must be an integer");
			return NULL;
		}
		i++;
	}
	num_ports = strtol(split, (char**) NULL, 10);
	printf("Number of output ports: %d\n", num_ports);

	//Probabilities
	double* probabilities = malloc(num_ports * sizeof(double));
	for (int count = 0; count < num_ports; count++) {
		int decimal = 0;
		split = strtok(NULL, " ");
		if (split == NULL) {
			printf("Not enough arguments for fork");
			free(probabilities);
			return NULL;
		}
		int i = 0;
		while (i < strlen(split)) {
			if (!isdigit(split[i])) {
				if (!decimal && split[i] == '.') {
					decimal = 1;
				} else {
					printf("Probabilities for fork must be a number");
					return NULL;
				}
			}
			i++;
		}
		if (split[i - 1] == '.') {
			//Nothing comes after the decimal point
			printf("Probabilities of fork are not valid");
			return NULL;
		}
		probabilities[count] = strtod(split, (char**) NULL);
		printf("Probability %d: %f\n", count, probabilities[count]);
	}

	//Output ids
	int *output_ids = malloc(num_ports * sizeof(int));
	for (int count = 0; count < num_ports; count++) {
		split = strtok(NULL, " ");
		if (split == NULL) {
			printf("Not enough arguments for fork");
			free(output_ids);
			free(probabilities);
			return NULL;
		}
		int i = 0;
		while (i < strlen(split)) {
			if (!isdigit(split[i])) {
				printf("Output ports for fork must be a number");
				return NULL;
			}
			i++;
		}
		output_ids[count] = strtod(split, (char**) NULL);
		printf("Output id %d: %d\n", count, output_ids[count]);
	}
	if (strtok(NULL, " ") != NULL) {
		free(probabilities);
		free(output_ids);
		printf("Too many arguments for fork component");
		return NULL;
	}
	addFork(id, probabilities, output_ids);
	printf("Created fork component\n\n");
	free(probabilities);
	free(output_ids);
	return ((void*) 1);
}

/*
 * Creates generator or queuing station, int gen indicates which one.
 * 1 = generator, 0 = queuing station
 */
void* create_g_q(int id, int gen) {
	char *split = strtok(NULL, " ");
	if (split == NULL) {
		if (gen) {
			printf(
					"Generator needs an average interarrival time and an output ID");
		} else {
			printf(
					"Queuing station needs an average service time and an output ID");
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
					printf("Interarrival time of generator should be a number");
				} else {
					printf(
							"Interarrival time of queuing station should be a number");
				}
				return NULL;
			}
		}
		i++;
	}
	if (split[i - 1] == '.') {
		//Nothing comes after the decimal point
		if (gen) {
			printf("Interarrival time of generator is incorrect");
		} else {
			printf("Service time of queuing station is incorrect");
		}
		return NULL;
	}
	double time = strtod(split, (char **) NULL);
	printf("Interarrival time: %f\n", time);
	split = strtok(NULL, " ");
	if (split == NULL) {
		if (gen) {
			printf(
					"Generator needs an average interarrival time and an output ID");
		} else {
			printf(
					"Queuing station needs an average service time and an output ID");
		}
		return NULL;
	}
	i = 0;
	int output_id;
	while (i < strlen(split)) {
		if (!isdigit(split[i])) {
			if (gen) {
				printf("Output ID of generator must be a number");
			} else {
				printf("Output ID of queuing station must be a number");
			}
			return NULL;
		}
		i++;
	}
	if (strtok(NULL, " ") != NULL) {
		if (gen) {
			printf("Too many arguments to generator component");
			return NULL;
		} else {
			printf("Too many arguments for queuing station component");
			return NULL;
		}
	}
	output_id = strtol(split, (char**) NULL, 10);
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

//Gets the ID of the component by reading first part of line
int get_id(char* buffer) {
	int i = 0;
	while (i < strlen(buffer)) {
		if (!isdigit(buffer[i])) {
			printf("Error: Each line must start with component ID");
			return -1;
		}
		i++;
	}
	return strtol(buffer, (char **) NULL, 10);
}

char get_component_type(char* buffer) {
	if (strlen(buffer) > 1) {
		printf("Component type should only be one letter");
		return '\0';
	}
	char ch = buffer[0];
	if (ch == 'G' || ch == 'E' || ch == 'Q' || ch == 'F') {
		return ch;
	} else {
		printf("Component type does not exist (%c)", ch);
		return '\0';
	}
}
