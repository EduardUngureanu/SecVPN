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

void listener()
{
    struct sigaction sa;
    struct sockaddr_in my_addr, cl_addr;
    int sock, sock_1, opt = 1;

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)) < 0)
    {
        syslog(LOG_ERR, "Cant't set socket options");
        exit(1);
    }

    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;

    // socket addres should be set here, find how

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
    {
        syslog(LOG_ERR, "Can't create socket");
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

    // use singals to avoid program termination, TODO
    memset(&sa,0,sizeof(sa));
    sa.sa_flags = SA_NOCLDWAIT;
    sa.sa_handler=sig_term;
    sigaction(SIGTERM,&sa,NULL);
    sigaction(SIGINT,&sa,NULL);
    server_term = 0;

    if ((sock_1 = accept(sock, (struct sockaddr *) &cl_addr, &opt)) < 0)
    {
	    syslog(LOG_ERR, "Error accepting connection");
	    exit(1);
    }
}

void connection(int sock)
{

}