#include "dropit.h"

static int          usage_error(char *prog_name)
{
    dprintf(2, "Usage:\n\t%1$s  -h\n\t%1$s [-p proto -f file] host port\n", prog_name);
    dprintf(2, "Options:\n\t-h: displays help\n\t-p: specify protocol in [tcp, http]\n");
    dprintf(2, "\t-f: specify the file to retreive on the HTTP server (mandatory if protocol is http)\n");
    return (-1);
}

static t_protocol   get_proto(char *arg)
{
    int         i;
    char        *proto_tab[] = {
        "tcp",
        "http",
        NULL
    };

    i = -1;
    while (proto_tab[++i])
        if (!strcmp(proto_tab[i], arg))
            return (i);
    dprintf(2, "Unknown protocol: %s\nSupported protocols are: \n\t- tcp\n\t- http\n", arg);
    return (PROTO_UNKNOWN);
}

static int      parse_remote(char **av, int index, t_config *config)
{
    int         ret;
    t_addrinfo  hints;

    bzero(&hints, sizeof(t_addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_CANONNAME;
    if ((ret = getaddrinfo(av[index], av[index + 1], &hints, &config->host_infos))) {
        dprintf(2, "Error during host resolution: %s\n", gai_strerror(ret));
        return (-1);
    }
    return (EXIT_SUCCESS);
}

int             parse_opt(int ac, char **av, t_config *config)
{
    int     opt;

    while ((opt = getopt(ac, av, "hp:f:")) != -1) {
        if (opt == 'p') {
            if ((config->protocol = get_proto(optarg)) == PROTO_UNKNOWN)
                return (-1);
        }
        else if (opt == 'f') {
            if (!(config->remote_file_name = strdup(optarg)))
                return (-1);
        }
        else if (opt == 'h')
            return (usage_error(av[0]));
    }
    if (optind != ac - 2 || parse_remote(av, optind, config) == -1)
        return (usage_error(av[0]));
    if (config->protocol == PROTO_HTTP && !config->remote_file_name) {
        dprintf(2, "The -f argument has to be used with http\n");
        return (-1);
    }
    return (EXIT_SUCCESS);
}