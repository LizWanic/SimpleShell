//------------------------------------------------------------------
// File: shell.c
//   
// Description: This program creates a simple command-line shell. 
//      1. It will display a simple prompt as shown:
//              prompt>
//      2. It will exit with status 0 when the user enters 'exit'.
//         It will cause a segmentation fault when the user enters
//         'explode' by calling fclose(NULL).
//      3. It will assume the command entered is one word with no
//         spaces or options.  
//         Therefore, it can handle:
//              prompt> ls
//         But is not required to handle:
//              prompt> ls - l
//      4. It will assume commands are less than 80 characters.
//      5. If an error occurs during fork or exec, a meaningful
//         error message will be displayed to the user, but the 
//         program will not terminate.
//      6. All error messages shall be sent to stderr.  Therefore,
//         either perror or fprintf shall be used. 
//      7. It will have a signal handler to catch the SIGSEGV signal.
//         When received, the handler will display the below and exit
//         with a code of -1. 
//              To display: 
//              A segmentation fault has been detected.
//              Exiting...
//     
//       
// Syntax: The program will be called from the command line using the 
//      following syntax: ./shell 
//       
//   
//------------------------------------------------------------------ 

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFF_SIZE 80
#define TRUE 1

// Signal handler function
void handler(int signum)
{
        switch (signum) {
                case SIGSEGV:
                        fprintf(stderr, "A segmentation fault has been detected.\n");
                        fprintf(stderr, "Exiting...\n");
                        fflush(stderr);
                        exit(-1);

                case SIGINT:
                        fprintf(stderr, "\nAn interrupt signal has been received.\n");
                        fprintf(stderr, "Exiting...\n");
                        fflush(stderr);
                        exit(0);

                case SIGTERM:
                        fprintf(stderr, "\nA termination signal has been received.\n");
                        fprintf(stderr, "Exiting...\n");
                        fflush(stderr);
                        exit(0);   
                default:
                        printf("Caught signal %d\n", signum);
                        exit(0);
        }

        fflush(stdout);
}

// Signal catcher function
void signals() {

        struct sigaction act;
        act.sa_handler = handler;
        sigemptyset(&act.sa_mask); 
        act.sa_flags = SA_RESTART;

        sigaction(SIGSEGV, &act, NULL);
        sigaction(SIGINT, &act, NULL);
        sigaction(SIGTERM, &act, NULL);

}

//------------------------------
// Main
//------------------------------

int main(int argc, char * argv[])
{
        // Creation of local variables

        char buffer[BUFF_SIZE];
        int pid;
        int length; 
        
        // Call the signal catching function
        signals();
        
        // While loop for the shell
        while (TRUE){

                // Print the prompt
                printf("prompt> ");

                // Get the input from the terminal
                fgets(buffer, BUFF_SIZE, stdin);

                // Handle just a carriage return
                if (!strncmp(buffer, "\n", 1)){
                        continue;
                }

                // Get length of input and replace the carraige return
                //      with a null terminator at the end of the string
                length = strlen(buffer);
                buffer[length-1] = '\0';

                // Handle the exit command
                if (!strncmp(buffer, "exit", 4)){
                        exit(0);

                // Handle the explode command
                } else if (!strncmp(buffer, "explode", 7)){
                        fclose(NULL);

                // Otherwise attempt to fork and exec
                } else {
                        pid = fork();

                        // Check for errors with forking
                        if (pid < 0) {
                                perror("Fork failed");
                        
                        // Exec based on the command
                        } else if (pid == 0) {
                                errno = 0;
                                execlp(buffer, buffer, NULL);
                                perror("Exec failed");
                                
                                // Handle exit command while in child
                                if (!strncmp(buffer, "exit", 4)){
                                        exit(0);
                                }

                                // Exit from the child
                                exit(0);

                        // Parent waits for the child to finish and then
                        //        the loop continues
                        } else {
                                pid = waitpid(pid, 0, 0);
                        }
                }    
            
                // Flush if needed (not sure if it is)
                fflush(stderr);
                fflush(stdout);

        }

        return (0); 

}

