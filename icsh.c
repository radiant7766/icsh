/* ICCS227: Project 1: icsh
 * Name: Aphithep Srichawla
 * StudentID: 6580003
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_CMD_BUFFER 255

int main() {
    char buffer[MAX_CMD_BUFFER];
    char last_buffer[MAX_CMD_BUFFER];
    char* username = getenv("USER"); 
    if (username == NULL) {
        username = "user";
    }
    while (1) {
        printf("%s@icsh ~ %% ", username);
        fgets(buffer, MAX_CMD_BUFFER, stdin);
        if (strncmp(buffer, "echo", 3) == 0) {
            printf("%s", (buffer+5));
        } else if (strcmp(buffer, "!!\n") == 0) {
            printf("%s", last_buffer);
        } else if (strncmp(buffer, "exit", 3) == 0) {
            if (isdigit(*(buffer+5))) {
                printf("goodbye!");
                return atoi((buffer+5));
            } else {
                printf("exit code must be numbers only!\n");
            }
        } else {
            if (strcmp(buffer, "\n") != 0) {
                printf("bad command\n");
            }
        }
        strcpy(last_buffer, buffer);
    }
}
