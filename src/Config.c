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
char get_component_type(char* buffer);

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
	if (!(read_file(ifp) == NULL)) {
		return 0;
	} else {
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
		buffer[buf_length-1] = '\0';

		//Read line of buffer
		if (read_line(buffer) == NULL) {
			fclose(ifp);
			free(ifp);
			free(buffer);
			return NULL;
		}

		//Reset buffer
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
	while (split != NULL) {
		if (count == 0) {
			//ID
			id = get_id(split);
			if(id==-1){
				return NULL;
			}
			printf("ID: %d\n", id);
		}
		if (count == 1){
			//Component type
			component_type = get_component_type(split);
			if(component_type == '\0'){
				return NULL;
			}
			printf("Component type: %c\n", component_type);
		}
		count++;
		split = strtok(NULL, " ");

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
	return atoi(buffer);
}

char get_component_type(char* buffer) {
	if(strlen(buffer)>1){
		printf("Component type should only be one letter");
		return '\0';
	}
	char ch = buffer[0];
	if(ch == 'G' || ch == 'E' || ch == 'Q' || ch == 'F'){
		return ch;
	} else {
		printf("Component type incorrect");
		return '\0';
	}
}
