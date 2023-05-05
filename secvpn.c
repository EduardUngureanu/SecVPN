#include <utils.h>
#include <getopt.h>

#define BUFF_SIZE 2000
#define PORT_NUMBER 55555
#define SERVER_IP "10.0.8.1"
#define SERVER_DEV "tap50"
#define CLIENT_DEV "tap51"

#define SERVER 0
#define CLIENT 1

int main(int argc, char *argv[]) {

    int tapfd, sockfd;
    char if_name[IFNAMSIZ] = "";
    char server_ip[16] = SERVER_IP;
    unsigned short int port = PORT_NUMBER;
    int mode = -1;

    int option;
    while ((option = getopt(argc, argv, "sc:p:i:")) > 0)
    {
        switch(option) {
            case 's':
                mode = SERVER;
                break;

            case 'c':
                mode = CLIENT;
                strncpy(server_ip, optarg, 15);
                break;

            case 'p':
                port = atoi(optarg);
                break;

            case 'i':
                strncpy(if_name, optarg, IFNAMSIZ-1);
                break;

            default:
                break;
        }
    }

    printf("Starting in %s mode...", (mode == 0) ? "server" : "client");

    if (*if_name == '\0') {
        if (mode == 0) {
            strncpy(if_name, SERVER_DEV, IFNAMSIZ-1);
        } else {
            strncpy(if_name, CLIENT_DEV, IFNAMSIZ-1);
        }
        printf("No interface name provided, using default %s", if_name);
    }

    if ((tapfd = tap_alloc(if_name)) < 0 ) {
        exit(1);
    }

    // socket creation goes here

    // get max descriptor
    int maxfd = (tapfd > sockfd) ? tapfd : sockfd;

    while (1) {
        fd_set readFDSet;
        int ret;

        FD_ZERO(&readFDSet);
        FD_SET(sockfd, &readFDSet);
        FD_SET(tapfd, &readFDSet);

        ret = select(maxfd, &readFDSet, NULL, NULL, NULL);

        if (ret < 0 && errno == EINTR) {
            continue;
        }

        if (ret < 0) {
            perror("select()");
            exit(1);
        }

        if(FD_ISSET(sockfd, &readFDSet)) {
            // read from sockfd, write to tunfd
        }

        if(FD_ISSET(tapfd, &readFDSet)) {
            // read from tunfd, write to sockfd

            int len;
            char buff[BUFF_SIZE];

            printf("Got a packet from TUN\n");

            bzero(buff, BUFF_SIZE); // may be unecessary

        }
    }

    return 0;
}