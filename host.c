#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    printf("My PID is %d\n", getpid());

    while (1) {
        printf("Hello from host process !\n");
        fflush(NULL);
        sleep(2);
    }
    return (EXIT_SUCCESS);
}
