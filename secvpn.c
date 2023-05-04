#include <utils.h>

#define BUFF_SIZE 2000
#define PORT_NUMBER 55555
#define SERVER_IP "10.0.8.1"

int main(int argc, char *argv[]) {

    int tunfd, sockfd; // set them using the fucntions

    // select max descriptor
    int maxfd = (tunfd > sockfd) ? tunfd : sockfd;

    while (1) {
        fd_set readFDSet;
        int ret;

        FD_ZERO(&readFDSet);
        FD_SET(sockfd, &readFDSet);
        FD_SET(tunfd, &readFDSet);

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

        if(FD_ISSET(tunfd, &readFDSet)) {
            // read from tunfd, write to sockfd

            int len;
            char buff[BUFF_SIZE];

            printf("Got a packet from TUN\n");

            bzero(buff, BUFF_SIZE); // may be unecessary

        }
    }

    return 0;
}