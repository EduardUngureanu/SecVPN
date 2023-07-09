#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <errno.h>
#include <signal.h>
#include <netinet/in.h>


#include "lib.h"

// Read from a socket (fd) with TCP protocol to a buffer (buf) and returns the length of the read
int tcp_read(int fd, char *buf)
{
    unsigned short len;

    // Read frame size
    if(read_n(fd, (char *)&len, sizeof(short)) < 0)
        return -1;

    return read_n(fd, buf, ntohs(len));
}

// Write len bytes from buf to fd, ensuring TCP protocol
int tcp_write(int fd, char *buf, int len)
{
    register char *ptr;

    ptr = buf - sizeof(short);
    *((unsigned short *)ptr) = htons(len);
    len = len + sizeof(short);

    return write_n(fd, ptr, len);
}