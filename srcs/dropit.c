#include "dropit.h"

int     main(int ac, char **av)
{
    t_config    config = {};

    if (parse_opt(ac, av, &config) == -1)
        return (EXIT_FAILURE);
    freeaddrinfo(config.host_infos);
    return (EXIT_SUCCESS);
}