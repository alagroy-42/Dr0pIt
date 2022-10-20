#include <unistd.h>
#include <stdlib.h>

int main(void)
{
    while (1) {
        system("echo I am running... >> /tmp/proof.txt");
        sleep(2);
    }
    return (0);
}