#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define USER_MODE_NORMAL 1
#define USER_MODE_NICED 2
#define	KERNAL_MODE 3
#define IDLE 4

void find_line_first_occurence(char *file_name, char *word, char *line){
	FILE *fin = fopen(file_name, "r");

	if(fin == NULL){
		perror("File doesn't open\n");
		exit(-1);
	}

	char buffer[128];

	while(fgets(buffer, 128, fin) != NULL){
		if(strstr(buffer, word) != NULL){
			strcpy(line, buffer);
			break;
		}
	}

	fclose(fin);
}

void convert_time_format(char* uptime, char time_type, int time){
	char buffer[16];
	if(time < 10) strcat(uptime, "0");

	if(time_type == 'S'){
		sprintf(buffer, "%d%c", time, time_type);
	} else {
	sprintf(buffer, "%d%c:", time, time_type);
	}
	
	strcat(uptime, buffer);
}

void read_file_processor(char *id, char *processor){
	FILE *fin = fopen("/proc/cpuinfo", "r");

	if(fin == NULL){
		perror("File doesn't open\n");
		exit(-1);
	}

	char buffer[256];
	
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
}

void read_file_version(char *version){
	FILE *fin = fopen("/proc/version", "r");

	if(fin == NULL){
		perror("File doesn't open\n");
		exit(-1);
	}
	
	fscanf(fin, "%*s %*s %s", version);
	fclose(fin);
}

void read_file_memory(char *memory){
	char buffer[128];

	find_line_first_occurence("/proc/meminfo", "MemTotal", buffer);
	sscanf(buffer, "%*s %s", memory);
}

double read_file_uptime(){
	double result = 0;
	char buffer[2][16];

	FILE *fin = fopen("/proc/uptime", "r");

	if(fin == NULL){
		perror("File doesn't open\n");
		exit(-1);
	}

	fscanf(fin, "%s %s", buffer[0], buffer[1]);
	return result = atof(buffer[0]) + atof(buffer[1]);

	fclose(fin);	
}

void sec_to_dhms(double sec, char *uptime){
	int dd = 0, hh = 0, mm = 0, ss = 0;
	int remainder = (int)sec;

	dd = remainder / 60 / 60 / 24;
	remainder = sec - (dd * 60 * 60 * 24);
	hh = remainder / 60 / 60;
	remainder = sec - (dd * 60 * 60 * 24) - (hh * 60 * 60);
	mm = remainder / 60;
	ss = sec - (dd * 60 * 60 * 24) - (hh * 60 * 60) - mm * 60;

	convert_time_format(uptime, 'D', dd);
	convert_time_format(uptime, 'H', hh);
	convert_time_format(uptime, 'M', mm);
	convert_time_format(uptime, 'S', ss);
}

double read_file_stat(char *file_name, int mode){
	FILE *fin = fopen(file_name, "r");

	if(fin == NULL){
		perror("File doesn't open\n");
		exit(-1);
	}

	char buffer[16];
	
	int i;
	for(i = 0; i <= mode; i++){
		fscanf(fin, "%s", buffer);
	}

	return atof(buffer) / 100;
}



double context_switch_num(){
	char buffer[32];
	char ctxt[32];

	find_line_first_occurence("/proc/stat", "ctxt", buffer);
	sscanf(buffer, "%*s %s", ctxt);

	return atof(ctxt);
}

double process_create_num(){
	char buffer[32];
	char process[32];

	find_line_first_occurence("/proc/stat", "processes", buffer);
	sscanf(buffer, "%*s %s", process);

	return atof(process);
}

double mem_free_num(){
	char buffer[32];
	char memory[32];

	find_line_first_occurence("/proc/meminfo", "MemFree", buffer);
	sscanf(buffer, "%*s %s", memory);

	//printf("%s\n", memory);

	return atof(memory);
}


double mem_total_num(){
	char buffer[32];
	char memory[32];

	find_line_first_occurence("/proc/meminfo", "MemTotal", buffer);
	sscanf(buffer, "%*s %s", memory);

	//printf("%s\n", memory);

	return atof(memory);
}

int main(int argc, char **argv){

	char *id, *processor;
	id = (char*)malloc(sizeof(char)*16);
	processor = (char*)malloc(sizeof(char)*512);

	id[0] = '\0';
	processor[0] = '\0';

	read_file_processor(id, processor);

	char *token_id, *token_processor, *save_id, *save_processor;
	token_id = strtok_r(id, "\n", &save_id);
	token_processor = strtok_r(processor, "\n", &save_processor);

	printf("-------------------------Processer Type-------------------------\n");

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
	read_file_version(version);
	
	printf("-------------------------Kernel Version-------------------------\n");
	printf("Linux version %s\n", version);

	free(version);


	char *memory = (char*)malloc(sizeof(char)*128);
	memory[0] = '\0';
	read_file_memory(memory);

	printf("------------------------Amount of Memory------------------------\n");
	printf("Installed Memory: %s KB\n", memory);	
	free(memory);

	
	char *uptime = (char*)malloc(sizeof(char)*32);
	uptime[0] = '\0';
	sec_to_dhms(read_file_uptime(), uptime);
	printf("----------------------Up Time Since Booted----------------------\n");	
	printf("%s\n", uptime);

	free(uptime);


	if(argc == 2){
		printf("----------------------------------------------------------------\n");
		printf("Observe period is: %ss\n", argv[1]);
		printf("----------------------------------------------------------------\n");
		while(1){
			double old_user_mode_time = read_file_stat("/proc/stat", USER_MODE_NORMAL) + read_file_stat("/proc/stat", USER_MODE_NICED);
			double old_kernal_mode_time = read_file_stat("/proc/stat", KERNAL_MODE);
			double old_idle_time = read_file_stat("/proc/stat", IDLE);
			double old_context_switch = context_switch_num();
			double old_process_create = process_create_num();
			sleep((unsigned int)atoi(argv[1]));
			double new_user_mode_time = read_file_stat("/proc/stat", USER_MODE_NORMAL) + read_file_stat("/proc/stat", USER_MODE_NICED);
			double new_kernal_mode_time = read_file_stat("/proc/stat", KERNAL_MODE);
			double new_idle_time = read_file_stat("/proc/stat", IDLE);
			double new_context_switch = context_switch_num();
			double new_process_create = process_create_num();
			int available_memory = (int)((mem_free_num() / mem_total_num()) * 100);	


			printf("Time (seconds) in user mode: %.2f\n\n", new_user_mode_time - old_user_mode_time);
			printf("Time (seconds) in sys mode: %.2f\n\n", new_kernal_mode_time - old_kernal_mode_time);
			printf("Time (seconds) in idle mode: %.2f\n\n", new_idle_time - old_idle_time);
			printf("Context switch rate (per minute): %.2f\n\n", ((new_context_switch - old_context_switch) / 60.0));
			printf("Processes created (per minute): %.2f\n\n", ((new_process_create - old_process_create) / 60.0));
			printf("The amount of available memory: %.0fKB\n\n", mem_free_num());
			printf("The amount of available memory: %d%%\n\n", available_memory);

			printf("----------------------------------------------------------------\n");
			
		}
	}

	return 0;
}