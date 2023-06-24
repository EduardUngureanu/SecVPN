#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "secvpn.h"

// TODO: Signal handling

void listener()
{
    struct sockaddr_in my_addr, cl_addr;
    int sock, sock_1, opt;

    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_addr.sin_port = htons(secvpn.bind_addr.port);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        syslog(LOG_ERR, "Can't create socket");
	    exit(1);
    }

    opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)) < 0)
    {
        syslog(LOG_ERR, "Cant't set socket options");
        exit(1);
    }

    if (bind(sock, (struct sockaddr *) &my_addr, sizeof(my_addr)))
    {
	    syslog(LOG_ERR, "Can't bind to the socket");
	    exit(1);
    }

    if (listen(sock, 10))
    {
	    syslog(LOG_ERR, "Can't listen on the socket");
	    exit(1);
    }

    syslog(LOG_INFO, "waiting for connections on port %d", secvpn.bind_addr.port);

    // TODO: Signal handling here for termination
    opt=sizeof(cl_addr);
    if ((sock_1 = accept(sock, (struct sockaddr *) &cl_addr, &opt)) < 0)
    {
        char str[INET_ADDRSTRLEN];
        syslog(LOG_ERR, "Error accepting connection from %s", inet_ntop(AF_INET, &cl_addr.sin_addr, str, sizeof(INET_ADDRSTRLEN)));
        exit(1);
    }

    close(sock);
    connection(sock_1);

    //TODO: Forking

    // switch (fork())
    // {
    // case 0:
    //     close(sock);
    //     connection(sock_1);
    //     break;
    // case -1:
    //     syslog(LOG_ERR, "Failed to fork()");
    // default:
    //     close(sock_1);
    //     break;
    // }
}

void connection(int sock)
{
    struct sockaddr_in my_addr, cl_addr;
    struct host *host;
    int opt;

    opt = sizeof(struct sockaddr_in);
    if(getpeername(sock, (struct sockaddr *) &cl_addr, &opt))
    {
        syslog(LOG_ERR, "Can't get peer name");
        exit(1);
    }

    opt = sizeof(struct sockaddr_in);
    if(getsockname(sock, (struct sockaddr *) &my_addr, &opt) < 0)
    {
        syslog(LOG_ERR, "Can't get local socket address");
        exit(1); 
    }

    //TODO: Authentification

    host->rmt_fd = sock;

    host->loc_addr = strdup(inet_ntoa(my_addr.sin_addr));
    host->loc_port = secvpn.bind_addr.port;
    host->rmt_addr = strdup(inet_ntoa(cl_addr.sin_addr));
    host->rmt_port = ntohs(cl_addr.sin_port);

    tunnel(host);
}

void server(int sock)
{
    //TODO: Signal ingnoiring for cutom signal handling

    syslog(LOG_INFO, "Starting server...");

    listener();
}