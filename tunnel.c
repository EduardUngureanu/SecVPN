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

// Alocate enough space for the buffer to keep an entire frame inside
static inline void * fdlink_buf_alloc(size_t size)
{
    register char * buf;

    size += FDLINK_FRAME_RESERV + FDLINK_FRAME_APPEND;

    if( !(buf = malloc(size)))
        return NULL;

    return buf + FDLINK_FRAME_RESERV; 
}

// Free the frame buffer
static inline void fdlink_buf_free(void *buf)
{
     unsigned char *ptr = buf;

     free(ptr - FDLINK_FRAME_RESERV);
}

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
    int fd1 = host->rmt_fd;
    int fd2 = host->loc_fd;
    fd_set fdset;
    int maxfd, len, plen;
    char *buf;

    if(!(buf = fdlink_buf_alloc(FRAME_SIZE)))
    {
	    syslog(LOG_ERR,"Can't allocate buffer for the linker"); 
        return 0; 
    }

    maxfd = (fd1 > fd2 ? fd1 : fd2) + 1;

    while(1)
    {
        errno = 0;

        FD_ZERO(&fdset);
        FD_SET(fd1, &fdset);
	    FD_SET(fd2, &fdset);

        if((len = select(maxfd, &fdset, NULL, NULL, NULL)) < 0)
        {
	        if(errno == EAGAIN || errno == EINTR)
                continue;
            else
                break;
        }

        
	    // Read frames from network (fd1) pass to local device (fd2)
        if(FD_ISSET(fd1, &fdset))
        {
            if((len = tcp_read(fd1, buf)) <= 0)
	            break;

            if((plen = write(fd2, buf, len)) < 0)
            {
	            if(errno == EAGAIN || errno == EINTR)
                    continue;
                else
                    break;
            }
        }

        // Read data from local device (fd2) pass to network (fd1)
        if(FD_ISSET(fd2, &fdset))
        {
            if((len = read(fd2, buf, FRAME_SIZE)) < 0)
            {
	            if(errno == EAGAIN || errno == EINTR)
                    continue;
                else
                    break;
            }

            if((plen = tcp_write(fd1, buf, len)) < 0)
                break;
        }
    }

    fdlink_buf_free(buf);

    return 0;
}