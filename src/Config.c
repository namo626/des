#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "Config.h"
#include "Sim.h"

/*
 * Functions
 */
int read_file(FILE* ifp);
int config(float time, char* configfile, char* outfile);

/*
 * Runs simulation
 */
int config(float time, char* configfile, char* outfile) {
	FILE* ifp;
	ifp = fopen(configfile, "r");
	if (ifp == NULL) {
		printf("Could not open file");
		return NULL;
	}

	int max_buf = 1000;
	char *buffer = malloc(sizeof(char) * max_buf);
	if (buffer == NULL) {
		//Malloc failed
		printf("Failed to allocate memory");
		fclose(ifp);
		free(ifp);
		return NULL;
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
				return NULL;
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
			return NULL;
		}
		i++;
	}
	num_components = atoi(buffer);
	initialize(num_components);

	//Reset buffer
	memset(buffer, 0, strlen(buffer));
	buf_length = 0;
	ch = '\0';

	//Read the rest of the file
	if (read_file(ifp) == NULL) {
		return NULL;
	} else {
		return 0;
	}
}

int read_file(FILE* ifp) {
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
		while ((ch != '\n')) {
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
		printf("%c", buffer[0]);

		//Do stuff with buffer

		//Reset buffer
		memset(buffer, 0, strlen(buffer));
		buf_length = 0;
	}

	fclose(ifp);
	free(ifp);
	free(buffer);
	return 0;
}

