#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h> 
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define	COLOR_NONE	"\033[0m"
#define	COLOR_GREEN	"\033[1;32m"
#define COLOR_RED	"\033[1:31m"

#define s_Size	15

#define TRUE	1

const char delim[]=" ";
char pwd[105];
char *path[s_Size];


void print_prompt();
void cmd_analysis(char *cmd,char **CMD,char **argv[]);
void init_path();
void set_path(char *argv[]);
void display_path();
void next_dir(char *argv[]);
void close_stdout(char *argv[],int pos);
void rum_cmd(char *argv[]);
void cmd_handle(char *argv[]);


int main()
{
	char cmd[55];
	init_path();
	while(TRUE)
	{
		char *argv[s_Size];
		print_prompt();
		gets(cmd);

		int i=0;;
		for(argv[i]=strtok(cmd,delim);	argv[i]!=NULL;	argv[i]=strtok(NULL,delim))	i++;
		
		cmd_handle(argv);
	}
}


void print_prompt()
{
	getcwd(pwd,105);
	printf(COLOR_GREEN"%s~wish> "COLOR_NONE,pwd);
}

void cmd_analysis(char *cmd,char **CMD,char **argv[])
{
}


void next_dir(char *argv[])
{
	if(argv[2]!=NULL || (-1==chdir(argv[0])))
		printf(COLOR_RED"no such file or directory\n"COLOR_NONE);	
}

void init_path()
{
	char tmp[]="/bin/";
	path[0]=(char*)malloc(sizeof(tmp));
	strcpy(path[0],tmp);
}
void set_path(char *argv[])
{
	int i,j;
	j=0;i=1;
	free_path();
	while(argv[i]!=NULL){
		path[j]=(char*)malloc(sizeof(argv[i])+1);
		strcat(argv[i],"/");
		strcpy(path[j],argv[i]);
		i++;
		j++;
	}
}
void free_path()
{
	int i=0;
	while(path[i]!=NULL){
		free(path[i]);
		i++;
	}
}
void display_path()
{
	int i=0;
	while(path[i]!=NULL)
		printf("%s\n",path[i++]);
}

bool cmd_exsit(char *argv[],char *tmp)
{
	int i=0;
	while(path[i]!=NULL){
		memset(tmp,0,sizeof(tmp));
		strcpy(tmp,path[i]);
		strcat(tmp,argv[0]);
		if(!access(tmp,X_OK)){
			return true;
		}
		i++;	
	}
}

void close_stdout(char *argv[],int pos)
{
	int fd=open(argv[pos+1],O_RDWR|O_CREAT|O_TRUNC,0664);
	dup2(fd,1);
	close(fd);
}

void run_cmd(char *argv[])
{
	int i=0;
	bool flag=false;
	char tmp[s_Size];
	char *argv_origin[s_Size];

	//argv[0] in path
	flag=cmd_exsit(argv,tmp);	

	if(flag==false){
		printf("An error has occurred\n");
		return ;	
	}

	//if exsit,separate it by >
	while(argv[i]!=NULL){
		if(strcmp(argv[i],">")){
			argv_origin[i]=(char*)malloc(sizeof(argv[i]));
			strcpy(argv_origin[i],argv[i]);
			i++;
		}
		else{
			flag=false;
			break;
		}
	}
	argv_origin[i]=NULL;

	pid_t pid=fork();
	if(pid==0){
		//redirction
		if(!flag){
			close_stdout(argv,i);		
			execv(tmp,argv_origin);
		}
		else	execv(tmp,argv);
	}
	else{
		waitpid(pid,NULL,0);
	}
}


void cmd_handle(char *argv[])
{
	if(!strcmp(argv[0],"exit"))	exit(0);
	else if(!strcmp(argv[0],"cd"))	next_dir(argv);
	else if(!strcmp(argv[0],"path")){
			set_path(argv);
			display_path();
	}
	else run_cmd(argv);	
}

