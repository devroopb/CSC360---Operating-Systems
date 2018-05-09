/*
  Devroop Banerjee
  V00837868
  CSC 360 - Assignment 1
  **NOTE ** The code I wrote is a concoction of the code provided in the
  appendices, and my research on google. I DID NOT copy code directly from
  the net; simply surfed the web for my queries and wrote my own version. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

//#define MAX_CMD_ARGS 9
//#define MAX_PROMPT_LEN 10
#define MAX_USER_INPUT 80
#define MAX_NUM_DIRECTORIES 10 

void openUVSHRC(char config[MAX_NUM_DIRECTORIES][256]){
	int i = 0;
	
	FILE *file;
	file = fopen(".uvshrc", "r");
	
	if (file){
		while(fscanf(file,"%s",&config[i][256]) != EOF){
			i++;
		}
		fprintf(stdout,"%s ",&config[0][256]);
		fclose(file);
		
	}else{
    		fprintf(stderr, "Oops! Something went wrong! Could not open file\n");
  	}	
}

void commands(char line[], int argc, char *argv[], char config[MAX_NUM_DIRECTORIES][256]){
	int counter = 0;
	char duplicate[MAX_USER_INPUT];
	char fullPath[MAX_USER_INPUT];      			//full file path
 	char *envp[] = {NULL};
	char *token;            						//split command into separate strings
			
	strcpy(duplicate, line);						//copy string to preserve original after tokenizing
	token = strtok(duplicate," ");
	int i = 0;
	
	while(token != NULL && i < MAX_USER_INPUT){
	    argv[i] = token;      
	    token = strtok(NULL," ");
	    i++;
	}

	argv[i] = NULL;							//set last value to NULL for execve
	argc = i;	
	strcpy(&config[0][256], "");					//so that user can use absolute paths instead of commands
	
	do{
		char *path = &config[counter][256];
		strcpy(fullPath, path);
		strcat(fullPath, "/");
		strcat(fullPath, argv[0]);				//add program to path
		if(execve(fullPath, argv, envp) == -1){
			counter++;
		}else{
			break;
		}
	}while(counter != MAX_NUM_DIRECTORIES);
	
	if(counter == MAX_NUM_DIRECTORIES){
		fprintf(stderr, "Sorry! \"%s\" does not exist or is not supported by the program.\n", line);
	}
}

void do_out(char line[], int argc, char *argv[], char config[MAX_NUM_DIRECTORIES][256]){
	char rough[256];
	char *extract, *arguments, *file;
	int fd;

	strcpy(rough, line);

	extract = strtok(rough," ");		//remove the "do-out"
	extract = strtok(NULL,"\n");

	arguments = strtok(extract,"::");	//extract the arguments to send to commands

	file = strtok(NULL,": ");		//extract file name to output the command

	fd = open(file, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	
	if(fd == -1){
		fprintf(stderr, "Please follow the following format: do-out [args] :: [filename]\n");
		exit(1);
	}else if(strstr(line, " :: ") == NULL){
		remove(file);
		fprintf(stderr, "Please use \" :: \" to redirect output to file\n");
		exit(1);
	}else if(strstr(line, ".txt") == NULL){
		remove(file);
		fprintf(stderr, "Please use a valid extension to save your file\n");
		exit(1);
	}else{
		printf("Cool... go check %s\n", file);
	}

	dup2(fd, 1);
	dup2(fd, 2);
	commands(arguments, argc, argv, config);
}

void do_pipe(char line[], int argc, char *argv[], char config[MAX_NUM_DIRECTORIES][256]){
	char rough[256];
	char *extract, *head, *tail;
	int fd[2];
	int pid_head, pid_tail, status;
	
	
	strcpy(rough, line);

	extract = strtok(rough," ");		//remove the "do-pipe"
	extract = strtok(NULL,"\n");
	head = strtok(extract,"::");		//extract the first set of arguments
	tail = strtok(NULL,":");			//extract the second set of arguments

	if((strstr(line, " :: ") == NULL) || (tail == NULL)){
		fprintf(stderr, "Please follow the following format: do-pipe [args] :: [args]\n");
		exit(1);
	}
	 /*
	   After this point, we now have a pipe in the form of two file
        descriptors. The end of the pipe from which we can read is
        fd[0], and the end of the pipe to which we write is fd[1].
        Any child created after this point will have these file 
        descriptors for the pipe.
     */
     pipe(fd);	
     if((pid_head = fork()) == 0){					
		dup2(fd[1], 1);
		close(fd[0]);
		commands(head, argc, argv, config);
	}
	if((pid_tail = fork()) == 0){
		dup2(fd[0], 0);
		close(fd[1]);
		commands(tail, argc, argv, config);
	}
	
	close(fd[0]);
	close(fd[1]);
	
	waitpid(pid_head, &status, 0);
	waitpid(pid_tail, &status, 0); 
}

int main(int argc, char *argv[]){
	char line[MAX_USER_INPUT];
	char config[MAX_NUM_DIRECTORIES][256];
	int pid;
    
	for(;;){
		openUVSHRC(config);
		fflush(stdout);
		fgets(line, MAX_USER_INPUT, stdin);

		if(line[strlen(line) - 1] == '\n'){
			line[strlen(line) - 1] = '\0';
		}
		
		if(strcmp(line, "exit") == 0){
			exit(0);
		}

		if ((pid = fork()) < 0){     
			fprintf(stderr, "Forking Failed!\n");
			break;

		}else if(pid == 0){

			if(strstr(line, "do-out ") != NULL){
				do_out(line, argc, argv, config);
			}else if(strstr(line, "do-pipe ") != NULL){
				do_pipe(line, argc, argv, config);
			}else{
				commands(line, argc, argv, config);
				break;	
			}
	    	}
	    	
		while (wait(&pid) > 0){
	    	}
	}
	return 0;
}
