#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "Config.h"
#include "Sim.h"

/*
 * Functions
 */
void* read_file(FILE* ifp);
int config(float time, char* configfile, char* outfile);
void *read_line(char* buffer);
int get_id(char* buffer);

/*
 * Runs simulation
 */
int config(float time, char* configfile, char* outfile) {
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
	num_components = atoi(buffer);
	initialize(num_components);

	//Read the rest of the file
	if (read_file(ifp) == NULL) {
		return 1;
	} else {
		return 0;
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

		//Read line of buffer
		if (read_line(buffer) == NULL) {
			return NULL;
		}

		//Reset buffer
		memset(buffer, 0, strlen(buffer));
		buf_length = 0;
	}

	fclose(ifp);
	free(ifp);
	free(buffer);
	return ((void*) 1);
}

void *read_line(char* buffer) {
	int id = get_id(buffer);
	if(id==-1){
		return NULL;
	}
	printf("%d\n",id);

	return ((void*)1);
}

int get_id(char* buffer){
	int i=0;
	while (buffer[i] != ' ') {
			if (!isdigit(buffer[i])) {
				printf(
						"Error: Each line must start with component ID");
				return -1;
			}
			i++;
		}
	buffer[i] = '\0';
	return atoi(buffer);
}
