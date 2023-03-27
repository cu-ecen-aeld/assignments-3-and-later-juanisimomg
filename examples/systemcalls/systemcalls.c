#include "systemcalls.h"

// For open and related flags.
#include <sys/stat.h>
#include <fcntl.h>

// For close.
#include <unistd.h>

// For system.
#include <stdlib.h>

// For wait.
#include <sys/wait.h>

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/
    int ret_sys = system(cmd);

    if (ret_sys != 0)
    {
        return false;
    }

    return true;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    // command[count] = command[count];

/*
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/
    va_end(args);
    fflush(stdout);
    pid_t child_pid = fork();

    if (child_pid == -1)
    {
        // We could not create the child.
        return false;
    }

    if (child_pid == 0)
    {
        // We are the child process.
        int child_error = execv(command[0], command);
        // We only get assigned errors if execv fails.
        printf("Child error is %d\n", child_error);
        // Parent expects 0 for success, so send -1.
        exit(-1);
    }
    else
    {
        // We are the parent process.
        // Wait for child process to finish and get their return.
        int child_status;
        int child_ret = waitpid(child_pid, &child_status, 0);
        printf("Parent: child_ret %d\n", child_ret);
        printf("Parent: child_status %d\n", child_ret);
        if (child_ret == -1)
        {
            // Execution failed.
            return false;
        }
        if (child_status != 0)
        {
            // Child failed.
            return false;
        }
    }

    // va_end(args);
    // We finished successfully.
    return true;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    // command[count] = command[count];


/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/

    va_end(args);

    // Start of redirection.
    // int kidpid;
    int fd = open(outputfile, O_WRONLY|O_TRUNC|O_CREAT, 0644);
    // End of redirection.
    fflush(stdout);
     pid_t child_pid = fork();

    if (child_pid == -1)
    {
        // We could not create the child.
        close(fd);
        return false;
    }

    if (child_pid == 0)
    {
        // We are the child process.
        // Redirect output.
        dup2(fd, 1);
        // Execute command.
        int child_error = execv(command[0], command);
        // We only get assigned errors if execv fails.
        printf("Child error is %d\n", child_error);
        // Parent expects 0 for success, so send -1.
        return -1;
    }
    else
    {
        // We are the parent process.
        // Wait for child process to finish and get their return.
        int child_status;
        int child_ret = waitpid(child_pid, &child_status, 0);
        if (child_ret == -1)
        {
            // Execution failed.
            return false;
        }
        if (child_status != 0)
        {
            // Child failed.
            return false;
        }
    }
    close(fd);
    return true;
}
