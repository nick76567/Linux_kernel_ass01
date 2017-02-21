#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define USER_MODE 0
#define	KERNAL_MODE 1
#define IDLE 2

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
	

	FILE *fin = fopen("/proc/uptime", "r");

	if(fin == NULL){
		perror("File doesn't open\n");
		exit(-1);
	}

	fscanf(fin, "%lf", &result);
	
	return result;
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

void read_file_stat(double* processor_mode_time){
	FILE *fin = fopen("/proc/stat", "r");

	if(fin == NULL){
		perror("File doesn't open\n");
		exit(-1);
	}

	double buffer[4];
	fscanf(fin, "%*s %lf %lf %lf %lf", &buffer[0], &buffer[1], &buffer[2], &buffer[3]);
	processor_mode_time[USER_MODE] = (buffer[0] + buffer[1]) / 100;
	processor_mode_time[KERNAL_MODE] = buffer[2] / 100;
	processor_mode_time[IDLE] = buffer[3] / 100;
	
	fclose(fin);
}

double read_file_num(char* file_name, char* target){
	char buffer[32];
	double result;

	find_line_first_occurence(file_name, target, buffer);
	sscanf(buffer, "%*s %lf", &result);

	return result;
}

int main(int argc, char **argv){
	if(argc == 1){
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
	}

	if(argc == 2){
		double old_mode_time[3], new_mode_time[3];
		double old_context_switch, new_context_switch, old_process_create, new_process_create, available_memory;

		printf("----------------------------------------------------------------\n");
		printf("Observe period is: %ss\n", argv[1]);
		printf("----------------------------------------------------------------\n");
		while(1){
			read_file_stat(old_mode_time);
			old_context_switch = read_file_num("/proc/stat", "ctxt");
			old_process_create = read_file_num("/proc/stat", "processes");
			sleep((unsigned int)atoi(argv[1]));
			read_file_stat(new_mode_time);
			new_context_switch = read_file_num("/proc/stat", "ctxt");
			new_process_create = read_file_num("/proc/stat", "processes");
			available_memory = ((read_file_num("/proc/meminfo", "MemFree") / read_file_num("/proc/meminfo", "MemTotal")) * 100);	

			printf("Time (seconds) in user mode: %.2f\n\n", new_mode_time[USER_MODE] - old_mode_time[USER_MODE]);
			printf("Time (seconds) in sys mode: %.2f\n\n", new_mode_time[KERNAL_MODE] - old_mode_time[KERNAL_MODE]);
			printf("Time (seconds) in idle mode: %.2f\n\n", new_mode_time[IDLE] - old_mode_time[IDLE]);
			printf("Context switch rate (per minute): %.2f\n\n", ((new_context_switch - old_context_switch) / 60.0));
			printf("Processes created (per minute): %.2f\n\n", ((new_process_create - old_process_create) / 60.0));
			printf("The amount of available memory: %.0fKB\n\n", read_file_num("/proc/meminfo", "MemFree"));
			printf("The amount of available memory: %.0f%%\n\n", available_memory);

			printf("----------------------------------------------------------------\n");
			
		}
	}

	return 0;
}