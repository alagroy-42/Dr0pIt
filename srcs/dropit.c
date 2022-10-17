#include "dropit.h"

int     main(int ac, char **av)
{
    t_dropit    dropit;

    if (parse_opt(ac, av, &dropit))
        return (EXIT_FAILURE);
    return (EXIT_SUCCESS);
}