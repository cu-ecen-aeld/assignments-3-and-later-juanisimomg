#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

int main(int argc, char *argv[]){
    if(argc != 3){
        // Insufficient arguments provided.
        return 1;
    }
    char *writefile = argv[1];
    char *writestr = argv[2];
    FILE * file_ptr;
    openlog(NULL, 0, LOG_USER);
    file_ptr = fopen(writefile, "w");
    if(file_ptr == NULL)
    {
        syslog(LOG_ERR, "Could not write to file: %s", writefile);
        return 1;
    }
    syslog(LOG_DEBUG, "Writing %s to %s", writestr, writefile);
    fputs(writestr, file_ptr);
    closelog();
    fclose(file_ptr);
    return 0;
}