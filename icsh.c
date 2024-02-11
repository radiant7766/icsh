/* ICCS227: Project 1: icsh
 * Name: Aphithep Srichawla
 * StudentID: 6580003
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

#define MAX_CMD_BUFFER 255
pid_t pid;
struct Node* head;
int dontprint = 0;

struct Node {
    int jobID;
    pid_t pid;
    int free;
    char command[255];
    struct Node* next;
};

struct Node* createNode(int jobID, pid_t pid, int free, char command[255]) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->jobID = jobID;
    newNode->pid = pid;
    newNode->free = free;
    strcpy(newNode->command, command);
    newNode->next = NULL;
    return newNode;
}
void handle_signal(int sig) 
{   
    if (sig == 2) {
        if (pid != 0) {
            printf("\n");
            kill(pid, SIGKILL);
        }
    } else if (sig == 18) {
        if (pid != 0) {
            printf("\n");
            kill(pid, SIGKILL);
        }
    } else {
        struct Node* j = head;
        int c = 1;
        while(c) {
            if (waitpid(j->pid, 0, WNOHANG) > 0) {
                j->free = 1;
                strcpy(j->command, "");
                c = 0;
            } else {
                j = j->next;
            }
        }             
    }
} 

int main(int argc, char *argv[]) {
    signal(SIGINT, handle_signal);
    signal(SIGTSTP, handle_signal);
    signal(SIGCHLD, handle_signal);
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
                        return 0;
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
    } else {
        head = createNode(1, 0, 1, "");
        char buffer[MAX_CMD_BUFFER];
        char last_buffer[MAX_CMD_BUFFER];
        char* username = getenv("USER"); 
        if (username == NULL) {
            username = "user";
        }
        while (1) {
            if (dontprint) {
                dontprint = 0;
            } else {
                printf("%s@icsh ~ %% ", username);
            }
            fgets(buffer, MAX_CMD_BUFFER, stdin);
            if (strchr(buffer, '>') != NULL) {
                char *command, *file;
                command = strtok(buffer, ">");
                file = strtok(NULL, "");
                file[strcspn(file, "\n")] = '\0';
                FILE *a = popen(command, "r");
                char buf[1000];
                FILE *fptr;
                fptr = fopen(file+1, "w");
                while (fgets(buf, sizeof(buf), a) != 0) {
                    fputs(buf, fptr);
                }
                pclose(a);
                fclose(fptr);
            } else if (strncmp(buffer, "echo", 4) == 0) {
                printf("%s", (buffer+5));
            } else if (strcmp(buffer, "!!\n") == 0) {
                printf("%s", last_buffer);
            } else if (strncmp(buffer, "exit", 4) == 0) {
                if (isdigit(*(buffer+5))) {
                    printf("%s\n", buffer);
                    return atoi((buffer+5));
                } else {
                    printf("exit code must be numbers only!\n");
                }
            } else if (strncmp(buffer, "fg", 2) == 0) {
                char *job = strchr(buffer, '%');
                if (job != NULL)
                {
                    job++;
                    int job_id = atoi(job);
                    struct Node* j = head;
                    int c = 1;
                    pid_t pid;
                    char command[255];
                    while(c) {
                        if (j->jobID == job_id) {
                            pid = j->pid;
                            strcpy(command, j->command);
                            c = 0;
                        } else {
                            if (j->next != NULL) {
                                j = j->next;
                            } else {
                                c = 0;
                            }
                        }
                    }
                    printf("%s", command);
                    dontprint = 1;
                    waitpid(pid, 0, WUNTRACED);
                }
            } else if (strncmp(buffer, "bg", 2) == 0) {
                kill(pid, SIGCONT);
                printf("continued %s", last_buffer);
            } else if (strcmp(buffer, "jobs\n") == 0) {
                struct Node* j = head;
                int c = 1;
                while(c) {
                    if (j->free == 0) {
                        printf("[%d]  Running        %s", j->jobID, j->command);
                        if (j->next != NULL) {
                            j = j->next;
                        } else {
                            c = 0;
                        }
                    } else {
                        if (j->next != NULL) {
                            j = j->next;
                        } else {
                            c = 0;
                        }
                    }
                }
            } else {
                if (strcmp(buffer, "\n") == 0) {

                } else {
                    size_t len = strlen(buffer);
                    if (buffer[len-2] == '&') {
                        struct Node* j = head;
                        int c = 1;
                        int a = 2;
                        int id = 0;
                        while(c) {
                            if (j->free) {
                                strcpy(j->command, buffer);
                                j->free = 0;
                                id = j->jobID;
                                c = 0;
                            } else {
                                if (j->next == NULL) {
                                    struct Node* newNode = createNode(a, 0, 0, buffer);
                                    j->next = newNode;
                                    id = a;
                                    c = 0;
                                } else {
                                    j = j->next;
                                    a++;
                                }
                            }
                        }
                        int x = fork();
                        if (x == 0) {
                            buffer[len-3] = '\n';
                            buffer[len-2] = '\0';
                            system(buffer);
                            printf("\n[%d] Done       %s\n", id, buffer);
                            printf("%s@icsh ~ %% ", username);
                            return 0;
                        } else {
                            printf("[%d] %d\n", id, x);
                            struct Node* j = head;
                            int c = 1;
                            while(c) {
                                if (j->jobID == id) {
                                    j->pid = x;
                                    c = 0;
                                } else {
                                    j = j->next;
                                }
                            }
                        }
                    } else {
                        int x = fork();
                        if (x == 0) {
                            system(buffer);
                            return 0;
                        } else {
                            pid = x;
                            waitpid(pid, 0, WUNTRACED);
                        }
                    }
                }
            }
            strcpy(last_buffer, buffer);
        }
    }
}
