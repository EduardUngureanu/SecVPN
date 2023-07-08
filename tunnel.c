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
#include <errno.h>
#include <netinet/tcp.h>
#include <sys/select.h>

#include "lib.h"
#include "secvpn.h"
#include "tun_tap.c"

int tunnel(struct host *host)
{
    int fd[2]={-1, -1};
    char dev[DEV_LEN]="";
    int opt;

    if(secvpn.dev_type == DEV_TAP)
    {
        if((fd[0] = tap_open(dev)) < 0)
        {
            syslog(LOG_ERR, "Can't allocate tap device %s. %s(%d)", dev, strerror(errno), errno);
            return -1;
        }
    } 
    else // if its not tap, its tun
    {
        if((fd[0] = tun_open(dev)) < 0)
        {
		    syslog(LOG_ERR,"Can't allocate tun device %s. %s(%d)", dev, strerror(errno), errno);
		    return -1;
        }
    }

    host->loc_fd = fd[0];
    host->dev = strdup(dev);

    // TODO: KEEPALIVE for down detection when implementing signal handling, possibly NODELAY for less latency
    // if(secvpn.protocol == PROTO_TCP)
    // {
    //     opt=1;
	//     setsockopt(host->rmt_fd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));

	//     opt=1;
	//     setsockopt(host->rmt_fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt)); 
    // }
    // else // if its not TCP, its UDP
    // {
    //     return 0;
    // }

    opt = fd_link(host);

    //TODO: see how to close and unlink tun/tap interfaces

    close(host->loc_fd);
    close(host->rmt_fd);
    close(fd[1]);

    return opt;
}

int fd_link(struct host *host)
{
    
}