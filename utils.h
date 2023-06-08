#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h> 
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
#include <stdarg.h>

int tap_alloc(char *dev) {
    struct ifreq ifr;
    int tapfd, err;
    char *clonedev = "/dev/net/tun";

    if ((tapfd = open(clonedev, O_RDWR)) < 0) {
        perror("Opening /dev/net/tun");
        return tapfd;
    }

    memset(&ifr, 0, sizeof(ifr));

    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;

    if (*dev) {
        strncpy(ifr.ifr_name, dev, IFNAMSIZ);
    }

    if ((err = ioctl(tapfd, TUNSETIFF, (void *)&ifr)) < 0) {
        perror("ioctl(TUNSETIFF)");
        close(tapfd);
        return err;
    }

    strcpy(dev, ifr.ifr_name);

    return tapfd;
}

// Initialize a server socket
int initServerSock(unsigned short int port_number) {

    int sockfd;
    struct sockaddr_in host;
    int optval = 1;

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval)) < 0) {
        perror("setsockopt()");
        exit(1);
    }

    memset(&host, 0, sizeof(host));
    host.sin_family = AF_INET;
    host.sin_addr.s_addr = htonl(INADDR_ANY);
    host.sin_port = htons(port_number);

    // using datagram, may need to use stream
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket()");
        exit(1);
    }

    if (bind(sockfd, (struct sockaddr *)&host, sizeof(host)) < 0) {
        perror("bind()");
        exit(1);
    }

    return sockfd;
}

// Create a socket connection to a server
int connectToServer(char *server_IP, unsigned short int port_number) {

    int sockfd;
    struct sockaddr_in remote;

    // destination socket
    memset (&remote, 0, sizeof(remote));
    remote.sin_family = AF_INET;
    remote.sin_addr.s_addr = inet_addr(server_IP);
    remote.sin_port = htons(port_number);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket()");
        exit(1);
    }

    if (connect(sockfd, (struct sockaddr*) &remote, sizeof(remote)) < 0) {
        perror("connect()");
        exit(1);
    }

    printf("CLIENT: Connected to server %s\n", inet_ntoa(remote.sin_addr));

    return sockfd;
}

// Return the file descriptor of the connection
int waitForConnection(int sockfd) {

    int netfd;
    struct sockaddr_in client;
    socklen_t clientlen;

    clientlen = sizeof(client);
    memset(&client, 0, clientlen);
    if ((netfd = accept(sockfd, (struct sockaddr*)&client, &clientlen)) < 0) {
        perror("accept()");
        exit(1);
    }

    printf("SERVER: Client connected from %s\n", inet_ntoa(client.sin_addr));

    return netfd;
}