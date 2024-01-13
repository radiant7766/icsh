/* ICCS227: Project 1: icsh
 * Name: Aphithep Srichawla
 * StudentID: 6580003
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_CMD_BUFFER 255

int main(int argc, char *argv[]) {
    if (argc > 3) {
        return 1;
    } else if (argc == 2) {
        char prevLine[100];
        char line[100];
        FILE *fptr;
        fptr = fopen(argv[1], "r");
        while(fgets(line, 100, fptr)) {
            if (strncmp(line, "echo", 3) == 0) {
                printf("%s", (line+5));
            } else if (strcmp(line, "!!\n") == 0) {
                printf("%s", prevLine);
            } else if (strncmp(line, "exit", 3) == 0) {
                if (isdigit(*(line+5))) {
                    printf("%d", atoi((line+5)));
                    return atoi((line+5));
                } else {
                    printf("exit code must be numbers only!\n");
                }
            } else {
                if (strcmp(line, "\n") != 0) {
                    printf("bad command\n");
                }
            }
            strcpy(prevLine, line);
        }
        return 0;
    } else {
        char buffer[MAX_CMD_BUFFER];
        char last_buffer[MAX_CMD_BUFFER];
        char* username = getenv("USER"); 
        if (username == NULL) {
            username = "user";
        }
        while (1) {
            printf("%s@icsh ~ %% ", username);
            fgets(buffer, MAX_CMD_BUFFER, stdin);
            if (strncmp(buffer, "echo", 4) == 0) {
                printf("%s", (buffer+5));
            } else if (strcmp(buffer, "!!\n") == 0) {
                printf("%s", last_buffer);
            } else if (strncmp(buffer, "exit", 4) == 0) {
                if (isdigit(*(buffer+5))) {
                    printf("goodbye!");
                    return atoi((buffer+5));
                } else {
                    printf("exit code must be numbers only!\n");
                }
            } else {
                if (strcmp(buffer, "\n") == 0) {
                    printf("\n");
                } else {
                    system(buffer);
                }
            }
            strcpy(last_buffer, buffer);
        }
    }
}
