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

void client(struct host *host)
{
    struct sockaddr_in my_addr, svr_addr;

    syslog(LOG_INFO, "Starting client...");
}