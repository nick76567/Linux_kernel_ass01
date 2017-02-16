#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void read_file_processor(char *file_name, char *id, char *processor){
	FILE *fin = fopen(file_name, "r");

	if(fin == NULL){
		perror("File doesn't open\n");
		exit(-1);
	}

	char *buffer = (char*)malloc(sizeof(char)*256);
	
	while(fgets(buffer, 256, fin) != NULL){
		if(strncmp(buffer, "processor", 9) == 0){
			char *target = strrchr(buffer, ':');
			strcat(id, target + 2);
		}

		if(strncmp(buffer, "model name", 10) == 0){
			char *target = strrchr(buffer, ':');
			strcat(processor, target + 2);
		}
	}

	fclose(fin);
	free(buffer);
}

void read_file_version(char *file_name, char *version){
	FILE *fin = fopen(file_name, "r");

	if(fin == NULL){
		perror("File doesn't open\n");
		exit(-1);
	}
	
	fgets(version, 256, fin);
	fclose(fin);
}

void read_file_memory(char *file_name, char *memory){
	FILE *fin = fopen(file_name, "r");

	if(fin == NULL){
		perror("File doesn't open\n");
		exit(-1);
	}

	char *buffer = (char*)malloc(sizeof(char)*128);


	while(fgets(buffer, 128, fin) != NULL){
		if(strncmp(buffer, "MemTotal", 8) == 0){
			char *target = strrchr(buffer, ':');
			strcpy(memory, target + 8);
			break;
		}
	}

	fclose(fin);
	free(buffer);
}

double read_file_uptime(char *file_name){
	double result = 0;
	char buffer[2][16];

	FILE *fin = fopen(file_name, "r");

	if(fin == NULL){
		perror("File doesn't open\n");
		exit(-1);
	}


	fscanf(fin, "%s %s", buffer[0], buffer[1]);
	return result = atof(buffer[0]) + atof(buffer[1]);	
}

void sec_to_dhms(double sec, char *uptime){
	int dd = 0, hh = 0, mm = 0, ss = 0;
	int remainder = (int)sec;
	char buffer[16];

	dd = remainder / 60 / 60 / 24;
	remainder = sec - (dd * 60 * 60 * 24);
	hh = remainder / 60 / 60;
	remainder = sec - (dd * 60 * 60 * 24) - (hh * 60 * 60);
	mm = remainder / 60;
	ss = sec - (dd * 60 * 60 * 24) - (hh * 60 * 60) - mm / 60;

		sprintf(buffer, "%d:", dd);
	if(dd < 10){
		strcat(uptime, "0");
	}
		strcat(uptime, buffer);

		sprintf(buffer, "%d:", hh);
	if(hh < 10){
		strcat(uptime, "0");
	}
		strcat(uptime, buffer);

		sprintf(buffer, "%d:", mm);
	if(mm < 10){
		strcat(uptime, "0");
	}
		strcat(uptime, buffer);

		sprintf(buffer, "%d", ss);
	if(ss < 10){
		strcat(uptime, "0");
	}
		strncat(uptime, buffer, 2);
}

int main(int argc, char **argv){
/*
	char *id, *processor;
	id = (char*)malloc(sizeof(char)*16);
	processor = (char*)malloc(sizeof(char)*512);

	id[0] = '\0';
	processor[0] = '\0';

	read_file_processor("/proc/cpuinfo", id, processor);

	char *token_id, *token_processor, *save_id, *save_processor;
	token_id = strtok_r(id, "\n", &save_id);
	token_processor = strtok_r(processor, "\n", &save_processor);

	printf("--------------------------Processor  Type---------------------------------------\n");

	while(token_id != NULL){
		printf("Processor-%s: %s\n", token_id, token_processor);
		token_id = strtok_r(NULL, "\n", &save_id);
		token_processor = strtok_r(NULL, "\n", &save_processor);
	}

	free(id);
	free(processor);

	char *version;
	version = (char*)malloc(sizeof(char)*256);
	version[0] = '\0';
	read_file_version("/proc/version", version);

	char *token = strtok(version, "(");
	token[strlen(token) - 1] = '\0';
	
	printf("---------------------------Kernel version---------------------------------------\n");
	printf("%s\n", token);

	free(version);



	char *memory = (char*)malloc(sizeof(char)*128);
	memory[0] = '\0';
	read_file_memory("/proc/meminfo", memory);

	printf("---------------------------Amount of memory-------------------------------------\n");
	printf("Installed Memory:%s", memory);	
	free(memory);

	
	char *uptime = (char*)malloc(sizeof(char)*32);
	uptime[0] = '\0';
	sec_to_dhms(read_file_uptime("/proc/uptime"), uptime);
	printf("---------------------------Up Time Since Booted----------------------------------\n");	
	printf("%s\n", uptime);

	free(uptime);
*/
	if(argc == 2){
		printf("----------------------------------------------------------------\n");
		printf("Observe period is: %ss\n", argv[1]);

		while(1){
			printf("----------------------------------------------------------------\n");
			
			printf("----------------------------------------------------------------\n");
			sleep((unsigned int)atoi(argv[1]));
		}
	}


	return 0;
}
