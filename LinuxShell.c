// omri azran 316098979
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <pwd.h>
#include <string.h>



// basic command struct
typedef struct{
    pid_t id;
    char commandStr[100];
} command;

// main 
void main(){
    // helper variables
    int i = 0;
    char buffer[100] = "";
    char* filterBuffer ; 
    int numOfCmds = 0;
    int isBackground ;
    int isEnter = 0;
    int isBuiltIn = 0;
    int retCode = 0;
    int numOfArgs = 0;
    int retStatus;
    char *tempStr;
    char strCmd[100];
    char cPath[100];
    char lPath[100];
    char cdArgs[100];
    char *homePath;
    char *tempPath;
    int chdirRetVal;
    int isCD ;
    command hisCmdArr[100] = {};
    char* cmdStrArr[100];
    pid_t pid;

    //first promt
    printf("$ ");
    fflush(stdout);

    //get user input
    fgets(buffer,100,stdin);
    // remove "\n" from input 
    filterBuffer = strtok(buffer,"\n");


   

    // main loop 
   while (strcmp(filterBuffer,"exit") != 0)
 {
    //init cmd string array
    memset(&cmdStrArr[0],0, sizeof(cmdStrArr));

    // init number of args counter
    numOfArgs = 0;
    // save command string before use of strtok
    strcpy(strCmd,buffer);

    // cut the input string and place each command arg in the cmdstrArr
    tempStr = strtok(filterBuffer, " ");
    while (tempStr != NULL)
    {
        cmdStrArr[numOfArgs] = tempStr;
        numOfArgs = numOfArgs + 1 ;
        tempStr = strtok(NULL," ");
    }

    // check if background char was in the input string and replace it with null
    if(cmdStrArr[numOfArgs -1] != NULL && strcmp(cmdStrArr[numOfArgs-1],"&") == 0){
      cmdStrArr[numOfArgs -1] = NULL;
      isBackground = 1;
      // if command is with background char remove it 
      strCmd[strlen(strCmd) -2] = '\0';
    }

    // saving command in history array
    strcpy(hisCmdArr[numOfCmds].commandStr,strCmd);


    // if history command was insert
    if (strcmp(cmdStrArr[0],"history") == 0)
    {    
        // built in cmd flag
        isBuiltIn = 1;
        //update pid
        hisCmdArr[numOfCmds].id = getpid();
        // print all commands and their status
        for (i = 0; i <= numOfCmds; i++)
        {  
            // the command
            printf("%s",hisCmdArr[i].commandStr);
            fflush(stdout);
            // the status
            if( i == numOfCmds  || waitpid(hisCmdArr[i].id,NULL,WNOHANG) == 0){
                printf(" RUNNING\n");
            }
            else{
                printf(" DONE\n");
            }
        }
        
    }

      //if jobs command was insert
    if (strcmp(cmdStrArr[0],"jobs") == 0)
    {   
        // built in cmd flag
        isBuiltIn = 1;
        //update pid
        hisCmdArr[numOfCmds].id = getpid();
        // print all commands with running status
        for (i = 0; i < numOfCmds; i++)
        {   
            if(waitpid(hisCmdArr[i].id,NULL,WNOHANG) == 0){
                printf("%s\n",hisCmdArr[i].commandStr);
            }
        }
    }
    //if cd command was insert
    if (strcmp(cmdStrArr[0],"cd") == 0)
    {
        // init cd flag
        isCD = 0 ;
        // built in cmd flag
        isBuiltIn = 1;
        //update pid
        hisCmdArr[numOfCmds].id = getpid();
        // invalid numbers of args check
        if (numOfArgs > 2)
        {
            printf("Too many arguments\n");
        }
        // if number of args is valid
        if (numOfArgs <= 2)
        {
           // Save the current path value to last path 
           strcpy(lPath,cPath);
           // get current path
           getcwd(cPath,sizeof(cPath));
           // check if pwd failed
           if (cPath == NULL)
           {
              printf("An error occurred\n");

           }
           if (cPath != NULL)
           {
               // save cd args
               strcpy(cdArgs,cmdStrArr[1]);

               // HOME case - '~'
               if (!strcmp(cmdStrArr[1],"~"))
               {
                   chdirRetVal = chdir(getenv("HOME"));
                   // chdir check
                   if (chdirRetVal == -1)
                   {
                       printf("chdir failed\n");
                   }
                   // cd flag 
                   isCD = 1;
                   
               }

               // HOME combination case 
               if (cdArgs[0] == '~' && isCD == 0)
               {
                    printf("in combination\n");
                    // Get Home path
                    homePath = getpwuid(getuid())->pw_dir;
                    // get the path after the '~'
                    tempPath=strtok(cdArgs,"~");
                    // concaetenate the home and the temp paths
                    strcat(homePath, tempPath);
                    //try chdir to the new home path
                    chdirRetVal = chdir(homePath);
                    // chdir check
                   if (chdirRetVal == -1)
                   {
                       printf("chdir failed\n");
                   }
                   // cd flag 
                   isCD = 1;
                   
               }
               // .. case
               if (!strcmp(cmdStrArr[1],".."))
               {
                   chdirRetVal= chdir("..");
                    // chdir check
                   if (chdirRetVal == -1)
                   {
                       printf("chdir failed\n");
                   }
                   // cd flag 
                   isCD = 1;
                   
               }
               // - case
               if (!strcmp(cmdStrArr[1],"-") && strlen(lPath) != 0)
               {
                    chdirRetVal = chdir(lPath);
                      // chdir check
                   if (chdirRetVal == -1)
                   {
                       printf("chdir failed\n");
                   }
                    // cd flag 
                    isCD = 1;
                   
               }
               // default case
               if (isCD != 1 && strlen(lPath) != 0 )
               {
                      chdirRetVal = chdir(cmdStrArr[1]);
                      // chdir check
                   if (chdirRetVal == -1)
                   {
                       printf("chdir failed\n");
                   }
               }   
               
           }
           
           
        }
        
        
    }
    

    //if non built in command was insert
    if (!isBuiltIn)
    {
        //echo check
        if(strcmp(cmdStrArr[0],"echo") == 0){
            // check if word is in quotation marks
            if (*cmdStrArr[1] == '"')
            {
                //remove  quotation marks
                cmdStrArr[1] = strtok(cmdStrArr[1],"\"");
                cmdStrArr[numOfArgs-1] = strtok(cmdStrArr[numOfArgs-1],"\"");
            }  

        }
        // create a process son
        pid = fork();
        // print error message if fork failed
        if (pid == -1) {
            printf("fork failed\n");
        }
        // if it is the son process 
        if (pid == 0)
        {   //try to pass the command to exec
            retCode = execvp(cmdStrArr[0], cmdStrArr);
             if (retCode == -1) {
                printf("exec failed\n");
                // exit son process if exec failed
                return;
            }
        }
        
        // if it is the father process
        if (pid != 0 && pid != -1)
        {
            //foreground case
            if (isBackground == 0) {
            waitpid(pid, &retStatus, 0);
            }
            // update father id
            hisCmdArr[numOfCmds].id=pid;
        }

          
        
        
    }

    //end of loop scan and enter check
    printf("$ ");
    fflush(stdout);

    // init buffer
    memset(&buffer[0], 0, sizeof(buffer));
    // get user input
    fgets(buffer, 100, stdin);
    filterBuffer = strtok(buffer, "\n");
    // init built in flag
    isBuiltIn = 0;
    isBackground = 0;
    //counter of commands
    numOfCmds = numOfCmds +1;
   
}
    
exit(0);

}


    
