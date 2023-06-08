#include <errno.h>
#include <signal.h>

// Read exactly len bytes
int read_n(int fd, char *buf, int len)
{
    int total = 0, nread;

    while (len > 0)
    {
        if ((nread = read(fd, buf, len)) < 0)
        {
            // if system call interupt or try again, dont exit
            if( errno == EINTR || errno == EAGAIN )
  	            continue;

            return -1;
        }

        if (!nread)
            return 0;

        len -= nread;
        buf += nread;
        total += nread;
    }

    return total;
}

// Write exactly len bytes
int write_n(int fd, char *buf, int len)
{
    int total = 0, nwrite;

    while (len > 0)
    {
        if ((nwrite = write(fd, buf, len)) < 0)
        {
            // if system call interupt or try again, dont exit
            if( errno == EINTR || errno == EAGAIN )
  	            continue;

            return -1;
        }

        if (!nwrite)
            return 0;

        len -= nwrite;
        buf += nwrite;
        total += nwrite;
    }

    return total;
}