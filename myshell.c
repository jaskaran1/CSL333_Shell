#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<string.h>
#include<fcntl.h>
#include<readline/readline.h>
#include<readline/history.h>
int parsestring(char str[],char *args[])
{
 int i=0;
 for(;i<10;i++)
 args[i]=NULL;//reinitialize it every time or else the previous command effect remains
 i=0;
 char* token;
 token=strtok(str," ");//str is modified.at the delimiter we have '\0'
 while(token!=NULL)
 {args[i++]=token;
  token=strtok(NULL," ");
 }
 return i;//the number of tokens    
}
    
void printparsed(char *args[],int n)
{int i;
 for(i=0;i<n;i++)
 printf("%s\n",args[i]);
}
int main(int argc, char *argv[], char *envp[])
{	pid_t pid;
	rl_bind_key('\t',rl_complete);//for auto completion
	char *comm;
	char copycomm[200];//copy of the comm command
	char *args[10];
	while((comm=readline("[jaskaran@shell]> "))!=NULL) {
	        strcpy(copycomm,comm);
	        if(strlen(comm)==0)
	               continue;
	        int notokens=parsestring(copycomm,args);
	        add_history(comm);
	        //printf("Nooftokens=%d\n",notokens);
	        //printparsed(args,notokens);
	        //printf("command=%s\n",comm);
	        //arg[0] has the utility
	        if(strcmp(comm,"exit")==0)//exit implemented
	        	    	 exit(EXIT_SUCCESS);
	        if(strcmp(args[0],"cd")==0)	
	        	pid=getpid();//get the process's own pid.no child process as cd isn't an executable
	        else    
	        	pid=fork();
	        	
	        if(pid==0){//the child process executes.first we check for >
	                	int i=0;
	                	for(;i<notokens;i++)
	        		{
	         		 if(strcmp(args[i],">")==0){
	                		if(i+1==notokens)
	                			{printf("Error:didn't give the filename to be written to\n");
	                 			 exit(EXIT_FAILURE);}//return from the child process
	                	        else if(i+2<notokens)
	                	        	{printf("Error:can't give any arguments after filename\n");
	                	        	 exit(EXIT_FAILURE);}//use exit instead of return to end the child process
	                	        else//only on argument after >
	                	        {int stdout_copy=dup(1);
	                	         close(1);//closes fd-1 which is stdout
	                	         int file=open(args[i+1],O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR);//opens a file with fd no 1 ie stdout points to the file
	                	         args[i]=NULL;//we also need to set the args[i] to NULL as we'll have to give args as a parameter to execvp and '>' won't be recognised by execvp since args is read only till NULL thus we have to do this. 
	                	         int status=execvp(args[0],args);//whatever is executed successfully is written there
	                	         close(file);
	                	         dup2(stdout_copy,1);
	                	         close(stdout_copy);
	                	         if(status<0)
	                	         	{printf("Error:command not found\n");
	                	         	 exit(EXIT_FAILURE);}
	                	         else
	                	         	exit(EXIT_SUCCESS);	   
	                	        }	
	         					   }
	      	
	         
	        	       }
	        	        int x=execvp(args[0],args);
	                 	//close(1);
	                	if(x<0)
	                		{printf("Error:command not found\n");
	                	 	 exit(EXIT_FAILURE);//returns to parent process
	                	 	}
	                	else
	                		exit(EXIT_SUCCESS); 	
	                }//it will automatically search for the program in path.This is done in a child process as calling exec,all the data in the calling program is lost
	        
	        if(pid>0){//we return to our shell interface
	        	wait();
	        	if(strcmp(args[0],"cd")==0)//cd implemented
	        	{//we can't execute cd as it isn't an executable
	        		int err;
	        		if(args[1]!=NULL)
	       	        		err=chdir(args[1]);
	       	        	if(err==-1)
	       	        		printf("Error:%s No file/directory\n",args[1]);
	       	        			}	
	         	}
	       	
	       if(pid<0)
	       		printf("Didn't fork\n"); 
	free(comm);
	}
	return 0;
}
