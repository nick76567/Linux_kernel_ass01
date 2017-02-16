// Reading command parameters
// Reference: http://stackoverflow.com/questions/5157337/c-reading-command-line-parameters
// Author: Nick Wong
// Date: 14-Feb-2017
// Content:
	bug. int main(void){}
	correct. int main(int argv, char **argv)){}
// Problem:
// 	To get the parameter like "[command name] [parameter]", we should use **argv.

// Using argv, **argv
// Author: Nick Wong
// Date 14-Feb-2017
// Content:
	bug. printf("%s\n", argv[1]);
	correct. if(i < argv) printf("%s\n", argv[i]);
// Problem:
// 	To prevent segmentation fault, we should check the number of argument we have.

// fgets(...)
// Reference: http://www.cplusplus.com/reference/cstdio/fgets/
// Author: Nick Wong
// Date: 14-Feb-2017
// Content:
	int i = 0;
        while(fgets(buffer, 100, fin) != NULL){
                strcpy(arr[i], buffer);
                i++;
        }
// Problem:
// 	fgets stops reading at newline or EOF.
// 	only EOF returns NULL
// 	Therefore, using NULL as the hint to pause the while loop
//
// 	To make it more convenient, we can use buffer to store the
// 	string temporarily

// malloc in function, free outside the function
// Author: Nick Wong
// Date: 15-Feb-2017
// Content:
	bug.	void read_file(char *file_name, char *id, char *processor){
				id = (char*)malloc(sizeof(char)*16);
				processor = (char*)malloc(sizeof(char)*256);
				.
				.
				.
			}

			int main(int argc, char **argv){

				
				char *token, *id, *processor;
				read_file("/proc/cpuinfo", id, processor);

				printf("%s\n", id);
				free(id);
				free(processor);


				return 0;
			}
	solve.	void read_file(char *file_name, char *id, char *processor){
					.
					.
					.
				}

				int main(int argc, char **argv){

					
					char *token, *id, *processor;
					id = (char*)malloc(sizeof(char)*16);
					processor = (char*)malloc(sizeof(char)*512);
					read_file("/proc/cpuinfo", id, processor);

					printf("%s\n", id);
					free(id);
					free(processor);


					return 0;
				}
// Problem:
//		segmentation fault??

// strtok(...)
// Referenc: http://www.cplusplus.com/reference/cstring/strtok/
// Author: Nick Wong
// Date: 15-Feb-2017
// Content:
//	bug.
		token = strtok(id, "\n");
		while(token != NULL){
			printf("%s\n", token);
			strtok(NULL, "\n");
		}
//  solve.
		token = strtok(id, "\n");
		while(token != NULL){
			printf("%s\n", token);
			strtok(NULL, "\n");
		}
//  2nd solve.
// Reference: http://stackoverflow.com/questions/9472865/tokenizing-multiple-strings-simultaneously
		char *token_id, *token_processor, *save_id, *save_processor;
		token_id = strtok_r(id, "\n", &save_id);
		token_processor = strtok_r(processor, "\n", &save_processor);

		while(token_id != NULL){
			printf("%s - %s\n", token_id, token_processor);
			token_id = strtok_r(NULL, "\n", &save_id);
			token_processor = strtok_r(NULL, "\n", &save_processor);
		}		
// Problem:
// Forget to update the value of token
// After the first of calling strtok, we can have to use NULL instead of using id
// it points to the address next to the specific string


// Malloc
// Author: Nick Wong
// Date: 16-Feb-2017
// Content:
// bug.
	char *uptime = (char*)malloc(sizeof(char)*32);
	sec_to_dhms(read_file_uptime("/proc/uptime"), uptime);
	printf("---------------------------Up Time Since Booted----------------------------------\n");	
	printf("%s\n", uptime);	
// solve
	char *uptime = (char*)malloc(sizeof(char)*32);
	uptime[0] = '\0';
	sec_to_dhms(read_file_uptime("/proc/uptime"), uptime);
	printf("---------------------------Up Time Since Booted----------------------------------\n");	
	printf("%s\n", uptime);
// Problem:
// print some strange word at beginner

