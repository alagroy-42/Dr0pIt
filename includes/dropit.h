#ifndef DROPIT_H
# define DROPIT_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <linux/memfd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <getopt.h>

#define NR_memfd_create         319

typedef struct user_regs_struct t_regs;
typedef struct sockaddr_in      t_sockaddr_in;
typedef struct sockaddr         t_sockaddr;
typedef unsigned char           byte;

typedef struct                  s_dropit
{
    in_addr_t       host;
    uint16_t        port;
}                               t_dropit;

int                             parse_opt(int ac, char **av, t_dropit *dropit);


#endif